#include <iostream>
#include <boost/bind.hpp>

#include "Client.h"

Client::Client(boost::asio::io_context& ioContext, const boost::asio::ip::address& ip, const int port)
    : m_socket(ioContext), m_endPoint(ip, port)
{
}

void Client::Start()
{
    m_socket.async_connect(m_endPoint,
        boost::bind(&Client::HandleConnect, shared_from_this(), boost::asio::placeholders::error));
}

void Client::HandleConnect(const boost::system::error_code& error)
{
    if (!error)
    {
        std::cout << "Connection is UP\n";
        boost::asio::async_read(m_socket,
            boost::asio::buffer(m_readMessage, sizeof(TMessage)),
            boost::bind(&Client::HandleRead, shared_from_this(), boost::asio::placeholders::error));
    }
    else
    {
        std::cout << "Unable to connect to the server.\nPAUSE\n";
    }
}

void Client::HandleRead(const boost::system::error_code& error)
{
    if (!error)
    {
        std::cout << "Connected clients: " << *(int *)m_readMessage << "\n";
        boost::asio::async_read(m_socket,
            boost::asio::buffer(m_readMessage, sizeof(TMessage)),
            boost::bind(&Client::HandleRead, shared_from_this(), boost::asio::placeholders::error));
    }
    else
    {
        Close();
    }
}

void Client::Close()
{
    m_socket.close();
}