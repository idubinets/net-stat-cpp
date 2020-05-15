#pragma once

#include <string>
#include <functional>
#include <memory>
#include <future>
#include <chrono>
#include <boost/asio.hpp>

#define HAVE_INT8_T
#define HAVE_STDINT_H
#include <net-snmp/net-snmp-config.h>
#undef snprintf
#define HAVE_SOCKLEN_T
#include <net-snmp/net-snmp-includes.h>

#ifdef HAVE_WINSOCK_H
#include <winsock.h>
#endif

struct SNMPResponse {
    std::string upTime;
    boost::system::error_code error;
};

using SNMPHandler = std::function<void(const std::shared_ptr<SNMPResponse>&, const boost::system::error_code&)>;

class SNMPClient : public std::enable_shared_from_this<SNMPClient>
{
public:
    SNMPClient(boost::asio::io_context& ioContext);
    ~SNMPClient();
    std::future<std::shared_ptr<SNMPResponse>> TestSystem(const std::string& ip, SNMPHandler handler = nullptr);

    std::shared_ptr<SNMPResponse> snmpResponse;

private:
    void Connect(const std::string& ip);
    int static AsynchResponse(int operation, struct snmp_session *snmpSession, int reqId,
        struct snmp_pdu *snmpPdu, void *magic);
    void AsyncSnmpGet(const std::string& snmpOid,
        std::shared_ptr<std::promise<std::shared_ptr<SNMPResponse>>> promise, SNMPHandler handler);
    void HandleSnmpRequest(struct snmp_pdu *snmpPdu, std::shared_ptr<std::promise<std::shared_ptr<SNMPResponse>>> promise,
        SNMPHandler handler, const boost::system::error_code &error);
    void HandleSnmpResponse(std::shared_ptr<std::promise<std::shared_ptr<SNMPResponse>>> promise,
        SNMPHandler handler, const boost::system::error_code &error);
    std::string MillisecondsToTime(std::chrono::milliseconds ms);
    void HandleDeadline();
    void HandleError(std::shared_ptr<std::promise<std::shared_ptr<SNMPResponse>>> promise, 
        const boost::system::error_code &error, SNMPHandler handler = nullptr);
        
    const long m_version;
    const std::string m_community;
    const std::string m_snmpOid;
    const int m_seconds;
    boost::asio::ip::udp::socket m_snmpSocket;
    boost::asio::steady_timer m_deadline;
    void* m_snmpHandle;
};
