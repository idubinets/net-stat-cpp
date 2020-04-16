#include "TMessage.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

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
	auto socket = boost::make_shared<tcp::socket>(io_context_);
	acceptor_.async_accept(*socket,
		boost::bind(&Server::HandleAccept, shared_from_this(), socket, boost::asio::placeholders::error));
}

void Server::HandleAccept(boost::shared_ptr<tcp::socket> socket, const boost::system::error_code& error)
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
		participants_.erase(socket);
		numberOfConnectedClients_--;
		MulticastNumberOfConnectedClients();
	}
}

void Server::Write(boost::shared_ptr<tcp::socket> socket, boost::shared_ptr<char[]>  buffer)
{
	boost::asio::async_write(*socket,
		boost::asio::buffer(&buffer[0], sizeof(TMessage<int>)),
		boost::bind(&Server::HandleWrite, shared_from_this(), boost::asio::placeholders::error));
}

void Server::MulticastNumberOfConnectedClients()
{
	boost::shared_ptr<char[]> buffer = boost::shared_ptr<char[]>(new char[sizeof(TMessage<int>)]);
	std::copy(static_cast<const char*>(static_cast<const void*>(&numberOfConnectedClients_)),
		static_cast<const char*>(static_cast<const void*>(&numberOfConnectedClients_)) + sizeof(TMessage<int>),
		&buffer[0]);
	std::for_each(participants_.begin(), participants_.end(),
		boost::bind(&Server::Write, shared_from_this(), _1, buffer));
}
