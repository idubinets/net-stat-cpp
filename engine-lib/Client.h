#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Client : public boost::enable_shared_from_this<Client> {
public:
	Client(boost::asio::io_context& io_context, const boost::asio::ip::address& ip, const int port);
	void Start();

private:
	void HandleConnect(const boost::system::error_code& error);
	void HandleRead(const boost::system::error_code& error);
	void Close();

private:
	tcp::endpoint endpoint_;
	tcp::socket socket_;
	enum { messageLength_ = 4 };
	char readMessage_[messageLength_];
};
