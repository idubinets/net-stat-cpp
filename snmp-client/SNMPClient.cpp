#include <boost/bind.hpp>

#include "SNMPClient.h"

SNMPClient::SNMPClient(boost::asio::io_context& ioContext)
    : m_snmpSocket(ioContext), m_snmpHandle(nullptr), m_version(SNMP_VERSION_2c), m_community("public"),
    m_snmpOid("1.3.6.1.2.1.1.3.0"), snmpResponse(std::make_shared<SNMPResponse>()), m_deadline(ioContext),
    m_seconds(3)
{
};

SNMPClient::~SNMPClient()
{
    snmp_sess_close(m_snmpHandle);
}

std::future<std::shared_ptr<SNMPResponse>> SNMPClient::TestSystem(const std::string& ip, SNMPHandler handler)
{
    Connect(ip);
    auto promise = std::make_shared<std::promise<std::shared_ptr<SNMPResponse>>>();
    auto future = promise->get_future();
    m_deadline.expires_after(std::chrono::seconds(m_seconds));
    AsyncSnmpGet(m_snmpOid, promise, handler);
    m_deadline.async_wait(boost::bind(&SNMPClient::HandleDeadline, shared_from_this()));
    return future;
}

void SNMPClient::HandleDeadline() 
{
    m_snmpSocket.close();
}

void SNMPClient::HandleError(std::shared_ptr<std::promise<std::shared_ptr<SNMPResponse>>> promise,
    const boost::system::error_code& error, SNMPHandler handler)
{
    m_deadline.cancel();
    snmpResponse->error = error;
    promise->set_value(snmpResponse);
    if (handler != nullptr) {
        boost::asio::post(boost::bind(handler, snmpResponse, error));
    }    
}

std::string SNMPClient::MillisecondsToTime(std::chrono::milliseconds ms)
{
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(ms);
    ms -= std::chrono::duration_cast<std::chrono::milliseconds>(secs);
    auto mins = std::chrono::duration_cast<std::chrono::minutes>(secs);
    secs -= std::chrono::duration_cast<std::chrono::seconds>(mins);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(mins);
    mins -= std::chrono::duration_cast<std::chrono::minutes>(hours);

    std::stringstream time;
    time << hours.count() << ":" << mins.count() << ":" << secs.count() << ":" << ms.count() / 10;
    return time.str();
}

int SNMPClient::AsyncResponse(int operation, struct snmp_session *snmpSession, int reqId,
    struct snmp_pdu *snmpPdu, void *magic)
{
    if (operation == NETSNMP_CALLBACK_OP_RECEIVED_MESSAGE && snmpPdu->errstat == SNMP_ERR_NOERROR) {
        if (snmpPdu->variables) {
            struct variable_list *vars = snmpPdu->variables;
            SNMPResponse* snmpResponse = static_cast<SNMPResponse*>(magic);
            snmpResponse->upTime = SNMPClient::MillisecondsToTime(static_cast<std::chrono::milliseconds>(vars->val.counter64->high * 10));
        }
    }
    return 1;
}

void SNMPClient::Connect(const std::string& ip)
{
    struct snmp_session snmpSession;
    snmp_sess_init(&snmpSession);
    snmpSession.version = m_version;
    snmpSession.peername = const_cast<char *>(ip.c_str());
    snmpSession.community = static_cast<u_char *>(static_cast<void *>(const_cast<char *>(m_community.c_str())));
    snmpSession.community_len = m_community.size();
    snmpSession.callback = AsyncResponse;
    snmpSession.callback_magic = snmpResponse.get();    
    m_snmpHandle = snmp_sess_open(&snmpSession);
    netsnmp_transport *transport = snmp_sess_transport(m_snmpHandle);
    m_snmpSocket.assign(boost::asio::ip::udp::v4(), transport->sock);
    m_snmpSocket.non_blocking();
}

void SNMPClient::AsyncSnmpGet(const std::string& snmpOid,
    std::shared_ptr<std::promise<std::shared_ptr<SNMPResponse>>> promise, SNMPHandler handler)
{    
    oid anOID[MAX_OID_LEN];
    size_t anOIDLength = MAX_OID_LEN;
    if (!snmp_parse_oid(snmpOid.c_str(), anOID, &anOIDLength)) {
        boost::system::error_code error = boost::system::errc::make_error_code(boost::system::errc::not_supported);
        HandleError(promise, error, handler);
        return;
    }
    struct snmp_pdu *snmpPdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(snmpPdu, anOID, anOIDLength);
    std::shared_ptr<struct snmp_pdu> snmpPduPtr(snmpPdu, [](snmp_pdu *pdu) {
        snmp_free_pdu(pdu);
    });
    m_snmpSocket.async_send(boost::asio::null_buffers(),
        boost::bind(&SNMPClient::HandleSnmpRequest, shared_from_this(), snmpPduPtr, promise, handler, boost::asio::placeholders::error));
}

void SNMPClient::HandleSnmpRequest(std::shared_ptr<snmp_pdu> snmpPdu, std::shared_ptr<std::promise<std::shared_ptr<SNMPResponse>>> promise,
    SNMPHandler handler, const boost::system::error_code &error)
{
    if (error) {
        HandleError(promise, error, handler);
    }
    else {
        snmp_sess_send(m_snmpHandle, snmpPdu.get());    
        m_snmpSocket.async_receive(boost::asio::null_buffers(),
            boost::bind(&SNMPClient::HandleSnmpResponse, shared_from_this(), snmpPdu, promise, handler, boost::asio::placeholders::error));
    }  
}

void SNMPClient::HandleSnmpResponse(std::shared_ptr<snmp_pdu> snmpPdu, std::shared_ptr<std::promise<std::shared_ptr<SNMPResponse>>> promise,
    SNMPHandler handler, const boost::system::error_code &error)
{    
    if (error) {
        HandleError(promise, error, handler);
    }
    else {
        m_deadline.cancel();
        fd_set snmpFds;
        FD_ZERO(&snmpFds);
        FD_SET(m_snmpSocket.native_handle(), &snmpFds);
        snmp_sess_read(m_snmpHandle, &snmpFds);
        promise->set_value(snmpResponse);
        if (handler != nullptr) {
            boost::asio::post(boost::bind(handler, snmpResponse, error));
        }        
    }   
}
