#pragma once

#include <set>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

class Server : public boost::enable_shared_from_this<Server> {
public:
	Server(boost::asio::io_context& io_context, const boost::asio::ip::address& ip, const int port);
	void Start();

private:
	void StartAccept();
	void HandleAccept(boost::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);
	void Read(boost::shared_ptr<tcp::socket> socket);
	void HandleRead(boost::shared_ptr<tcp::socket> socket, std::string* readMessage, const boost::system::error_code& error);
	void Write(boost::shared_ptr<tcp::socket> socket, boost::shared_ptr<char[]>  buffer);
	void HandleWrite(const boost::system::error_code& error) {}
	void MulticastNumberOfConnectedClients();

private:
	std::set<boost::shared_ptr<tcp::socket>> participants_;
	boost::asio::io_context& io_context_;
	tcp::acceptor acceptor_;
	int numberOfConnectedClients_;
	enum { messageLength_ = 4 };
};
