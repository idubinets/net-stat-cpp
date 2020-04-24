#pragma once

#include <set>
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

class Server : public boost::enable_shared_from_this<Server> {
public:
	Server(boost::asio::io_context& ioContext, const boost::asio::ip::address& ip, const int port);
	void Start();

private:
	void StartAccept();
	void HandleAccept(boost::shared_ptr<tcp::socket> socket, const boost::system::error_code& error);
	void Read(boost::shared_ptr<tcp::socket> socket);
	void HandleRead(boost::shared_ptr<tcp::socket> socket, boost::shared_ptr<char[]> readMessage, const boost::system::error_code& error);
	void Write(boost::shared_ptr<tcp::socket> socket, boost::shared_ptr<char[]>  buffer);
	void HandleWrite(const boost::system::error_code& error) {}
	void MulticastNumberOfConnectedClients();

private:
	std::set<boost::shared_ptr<tcp::socket>> m_participants;
	boost::asio::io_context& m_ioContext;
	boost::asio::ip::tcp::endpoint m_endPoint;
	tcp::acceptor m_acceptor;
	int m_numberOfConnectedClients;
};
