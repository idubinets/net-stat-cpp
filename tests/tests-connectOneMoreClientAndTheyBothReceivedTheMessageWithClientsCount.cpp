#include <Server.h>
#include <Client.h>

#include <catch2/catch.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <iostream>

TEST_CASE("Connect one more client and they both received the message with \"clients count\"", 
	"[server][client][server start][client start][client connect][client get message]")
{
	std::stringstream stream;
	std::streambuf* oldStream = std::cout.rdbuf(stream.rdbuf());
	std::string ip = "127.0.0.1";
	int port = 30001;
	boost::asio::io_context io_context_server;
	auto server = boost::make_shared<Server>(io_context_server, boost::asio::ip::make_address(ip), port);

	server->Start();
	boost::asio::io_context io_context_client_1;
	auto client_1 = boost::make_shared<Client>(io_context_client_1, boost::asio::ip::make_address(ip), port);
	client_1->Start();

	io_context_client_1.run_one();
	io_context_server.run_one();
	io_context_client_1.run_one();
	io_context_server.run_one();

	boost::asio::io_context io_context_client_2;
	auto client_2 = boost::make_shared<Client>(io_context_client_2, boost::asio::ip::make_address(ip), port);
	client_2->Start();

	io_context_client_2.run_one();
	io_context_server.run_one();
	io_context_client_1.run_one();
	io_context_client_2.run_one();

	std::string text = stream.str();
	REQUIRE(text == "Connection is UP\nConnected clients: 1\nConnection is UP\nConnected clients: 2\nConnected clients: 2\n");

	std::cout.rdbuf(oldStream);
}
