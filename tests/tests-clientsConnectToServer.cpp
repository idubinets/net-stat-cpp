#include <Server.h>
#include <Client.h>

#include <catch2/catch.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <iostream>

TEST_CASE("Clients connect to server") 
{		
	std::stringstream stream;
	std::streambuf* oldStream = std::cout.rdbuf(stream.rdbuf());
	std::string ip = "127.0.0.1";
	int port = 30001;
	boost::asio::io_context io_context_server;
	auto server = boost::make_shared<Server>(io_context_server, boost::asio::ip::make_address(ip), port);

	SECTION("Server can start listening to connections") 
	{		
		REQUIRE_NOTHROW(server->Start());
	}

	SECTION("Client can connect to the server")
	{
		server->Start();
		boost::asio::io_context io_context_client;
		auto client = boost::make_shared<Client>(io_context_client, boost::asio::ip::make_address(ip), port);
		REQUIRE_NOTHROW(client->Start());

		io_context_client.run_one();
		std::string text = stream.str();
		REQUIRE(text == "Connection is UP\n");
	}

	SECTION("Client gets information from the server about the amount of connected clients")
	{
		server->Start();
		boost::asio::io_context io_context_client;
		auto client = boost::make_shared<Client>(io_context_client, boost::asio::ip::make_address(ip), port);
		client->Start();
		
		io_context_client.run_one();
		io_context_server.run_one();
		io_context_client.run_one();
		std::string text = stream.str();
		REQUIRE(text == "Connection is UP\nConnected clients: 1\n");		
	}

	SECTION("Connect one more client and make sure they both received the message with \"clients count\"")
	{
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
	}

	SECTION("Disconnect one of the client and make sure that the other connected clients gets the message with \"clients count\"")
	{
		server->Start();
		boost::asio::io_context io_context_client_1;
		auto client_1 = boost::make_shared<Client>(io_context_client_1, boost::asio::ip::make_address(ip), port);
		client_1->Start();
				
		io_context_client_1.run_one();
		io_context_server.run_one();
		io_context_client_1.run_one();
		io_context_server.run_one();

		std::thread t([&]() {
			boost::asio::io_context io_context_client_2;
			auto client_2 = boost::make_shared<Client>(io_context_client_2, boost::asio::ip::make_address(ip), port);
			client_2->Start();
		});
		t.join();

		io_context_server.run_one();
		io_context_client_1.run_one();
		io_context_server.run_one();
		io_context_server.run_one();
		io_context_server.run_one(); 
		io_context_client_1.run_one();

		std::string text = stream.str();
		REQUIRE(text == "Connection is UP\nConnected clients: 1\nConnected clients: 2\nConnected clients: 1\n");
	}

	std::cout.rdbuf(oldStream);
}
