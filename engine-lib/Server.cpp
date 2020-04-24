#include "TMessage.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include "Server.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& ioContext, const boost::asio::ip::address& ip, const int port)
    : m_ioContext(ioContext), m_endPoint(ip, port), m_acceptor(ioContext), m_numberOfConnectedClients(0)
{
}

void Server::Start()
{
    m_acceptor.open(m_endPoint.protocol());
    m_acceptor.bind(m_endPoint);
    m_acceptor.listen();
    StartAccept();
}

void Server::StartAccept()
{
    auto socket = boost::make_shared<tcp::socket>(m_ioContext);
    m_acceptor.async_accept(*socket,
        boost::bind(&Server::HandleAccept, shared_from_this(), socket, boost::asio::placeholders::error));
}

void Server::HandleAccept(boost::shared_ptr<tcp::socket> socket, const boost::system::error_code& error)
{
    if (error.value() == boost::asio::error::operation_aborted) 
    {
        return;
    }
    if (!error)
    {
        m_participants.insert(socket);
        m_numberOfConnectedClients++;
        MulticastNumberOfConnectedClients();
        Read(socket);
    }
    StartAccept();
}

void Server::Read(boost::shared_ptr<tcp::socket> socket)
{
    boost::shared_ptr<char[]> readMessage = boost::shared_ptr<char[]>(new char[1]);
    boost::asio::async_read(*socket,
        boost::asio::buffer(&readMessage[0], 1),
        boost::bind(&Server::HandleRead, shared_from_this(), socket, readMessage, boost::asio::placeholders::error));
}

void Server::HandleRead(boost::shared_ptr<tcp::socket> socket, boost::shared_ptr<char[]> readMessage, const boost::system::error_code& error)
{
    if (error)
    {
        m_participants.erase(socket);
        m_numberOfConnectedClients--;
        MulticastNumberOfConnectedClients();
    }
}

void Server::Write(boost::shared_ptr<tcp::socket> socket, boost::shared_ptr<char[]> buffer)
{
    boost::asio::async_write(*socket,
        boost::asio::buffer(&buffer[0], sizeof(TMessage)),
        boost::bind(&Server::HandleWrite, shared_from_this(), boost::asio::placeholders::error));
}

void Server::MulticastNumberOfConnectedClients()
{
    boost::shared_ptr<char[]> buffer = boost::shared_ptr<char[]>(new char[sizeof(TMessage)]);
    std::copy(static_cast<const char*>(static_cast<const void*>(&m_numberOfConnectedClients)),
        static_cast<const char*>(static_cast<const void*>(&m_numberOfConnectedClients)) + sizeof(TMessage),
        &buffer[0]);
    std::for_each(m_participants.begin(), m_participants.end(),
        boost::bind(&Server::Write, shared_from_this(), _1, buffer));
}
