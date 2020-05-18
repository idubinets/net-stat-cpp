#pragma once

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "TMessage.h"

using boost::asio::ip::tcp;

class Client : public boost::enable_shared_from_this<Client> {
public:
    Client(boost::asio::io_context& ioContext, const boost::asio::ip::address& ip, const int port);
    void Start();

private:
    void HandleConnect(const boost::system::error_code& error);
    void HandleRead(const boost::system::error_code& error);
    void Close();

private:
    tcp::endpoint m_endPoint;
    tcp::socket m_socket;
    char m_readMessage[sizeof(TMessage)];
};
