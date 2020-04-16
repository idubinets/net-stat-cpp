#include <iostream>
#include <memory>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "Client.h"

Client::Client(boost::asio::io_context& io_context, const boost::asio::ip::address& ip, const int port)
	: socket_(io_context), endpoint_(ip, port)
{
}

void Client::Start()
{
	socket_.async_connect(endpoint_,
		boost::bind(&Client::HandleConnect, this->shared_from_this(), boost::asio::placeholders::error));
}

void Client::HandleConnect(const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "Connection is UP\n";
		boost::asio::async_read(socket_,
			boost::asio::buffer(readMessage_, messageLength_),
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
		std::cout << "Connected clients: " << *(int *)readMessage_ << "\n";
		boost::asio::async_read(socket_,
			boost::asio::buffer(readMessage_, messageLength_),
			boost::bind(&Client::HandleRead, this->shared_from_this(), boost::asio::placeholders::error));
	}
	else
	{
		Close();
	}
}

void Client::Close()
{
	socket_.close();
}