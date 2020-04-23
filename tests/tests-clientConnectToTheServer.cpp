#include <Server.h>
#include <Client.h>

#include <catch2/catch.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <iostream>

TEST_CASE("Client connect to the server", 
	"[server][client][server start][client start][client connect]")
{
	std::stringstream stream;
	std::streambuf* oldStream = std::cout.rdbuf(stream.rdbuf());
	std::string ip = "127.0.0.1";
	int port = 30001;
	boost::asio::io_context io_context_server;
	auto server = boost::make_shared<Server>(io_context_server, boost::asio::ip::make_address(ip), port);

	server->Start();
	boost::asio::io_context io_context_client;
	auto client = boost::make_shared<Client>(io_context_client, boost::asio::ip::make_address(ip), port);
	REQUIRE_NOTHROW(client->Start());

	io_context_client.run_one();
	std::string text = stream.str();
	REQUIRE(text == "Connection is UP\n");

	std::cout.rdbuf(oldStream);
}
