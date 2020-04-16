#pragma once

#include <set>
#include <string>
#include <memory>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Server : public std::enable_shared_from_this<Server> {
public:
	Server(boost::asio::io_context& io_context, const boost::asio::ip::address& ip, const int port);
	void Start();

private:
	void StartAccept();
	void HandleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);
	void Read(std::shared_ptr<tcp::socket> socket);
	void HandleRead(std::shared_ptr<tcp::socket> socket, std::string* readMessage, const boost::system::error_code& error);
	void Write(std::shared_ptr<tcp::socket> socket, std::shared_ptr<char[]>  buffer);
	void HandleWrite(const boost::system::error_code& error) {}
	void MulticastNumberOfConnectedClients();

private:
	std::set<std::shared_ptr<tcp::socket>> participants_;
	boost::asio::io_context& io_context_;
	tcp::acceptor acceptor_;
	int numberOfConnectedClients_;
	enum { messageLength_ = 4 };
};
