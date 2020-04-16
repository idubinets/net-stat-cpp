#include <iostream>
#include <memory>
#include <set>
#include <string>

#include "Server.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context, const boost::asio::ip::address& ip, const int port)
	: io_context_(io_context), acceptor_(io_context, tcp::endpoint(ip, port)), numberOfConnectedClients_(0)
{
}

void Server::Start()
{
	StartAccept();
}

void Server::StartAccept()
{
	auto socket = std::make_shared<tcp::socket>(io_context_);
	acceptor_.async_accept(*socket,
		boost::bind(&Server::HandleAccept, this->shared_from_this(), socket, boost::asio::placeholders::error));
}

void Server::HandleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error)
{
	if (!error)
	{
		participants_.insert(socket);
		numberOfConnectedClients_++;
		MulticastNumberOfConnectedClients();
		Read(socket);
	}
	StartAccept();
}

void Server::Read(std::shared_ptr<tcp::socket> socket)
{
	std::string* readMessage = new std::string();
	boost::asio::async_read(*socket,
		boost::asio::buffer(readMessage, 1),
		boost::bind(&Server::HandleRead, this->shared_from_this(), socket, readMessage, boost::asio::placeholders::error));
}

void Server::HandleRead(std::shared_ptr<tcp::socket> socket, std::string* readMessage, const boost::system::error_code& error)
{
	if (error)
	{
		delete readMessage;
		participants_.erase(socket);
		numberOfConnectedClients_--;
		MulticastNumberOfConnectedClients();
	}
}

void Server::Write(std::shared_ptr<tcp::socket> socket, std::shared_ptr<char[]>  buffer)
{
	boost::asio::async_write(*socket,
		boost::asio::buffer(&buffer[0], messageLength_),
		boost::bind(&Server::HandleWrite, this->shared_from_this(), boost::asio::placeholders::error));
}

void Server::MulticastNumberOfConnectedClients()
{
	std::shared_ptr<char[]> buffer = std::shared_ptr<char[]>(new char[messageLength_]);
	std::copy(static_cast<const char*>(static_cast<const void*>(&numberOfConnectedClients_)),
		static_cast<const char*>(static_cast<const void*>(&numberOfConnectedClients_)) + messageLength_,
		&buffer[0]);
	std::for_each(participants_.begin(), participants_.end(),
		boost::bind(&Server::Write, this->shared_from_this(), _1, buffer));
}
