#include <Server.h>
#include <Client.h>

#include <catch2/catch.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <iostream>

TEST_CASE("Server starts listening to connections",
    "[server][start]")
{
    std::stringstream stream;
    std::streambuf* oldStream = std::cout.rdbuf(stream.rdbuf());
    std::string ip = "127.0.0.1";
    int port = 30001;
    boost::asio::io_context ioContextServer;
    auto server = boost::make_shared<Server>(ioContextServer, boost::asio::ip::make_address(ip), port);

    REQUIRE_NOTHROW(server->Start());

    std::cout.rdbuf(oldStream);
}

TEST_CASE("Client connects to the server", 
    "[client][connect][single]")
{
    std::stringstream stream;
    std::streambuf* oldStream = std::cout.rdbuf(stream.rdbuf());
    std::string ip = "127.0.0.1";
    int port = 30001;
    boost::asio::io_context ioContextServer;
    auto server = boost::make_shared<Server>(ioContextServer, boost::asio::ip::make_address(ip), port);

    server->Start();
    boost::asio::io_context ioContextClient;
    auto client = boost::make_shared<Client>(ioContextClient, boost::asio::ip::make_address(ip), port);
    REQUIRE_NOTHROW(client->Start());

    ioContextClient.run_one();
    std::string text = stream.str();
    REQUIRE(text == "Connection is UP\n");

    std::cout.rdbuf(oldStream);
}

TEST_CASE("Client gets information from the server about the amount of connected clients",
    "[client][single][message][on-connect]")
{
    std::stringstream stream;
    std::streambuf* oldStream = std::cout.rdbuf(stream.rdbuf());
    std::string ip = "127.0.0.1";
    int port = 30001;
    boost::asio::io_context ioContextServer;
    auto server = boost::make_shared<Server>(ioContextServer, boost::asio::ip::make_address(ip), port);

    server->Start();
    boost::asio::io_context ioContextClient;
    auto client = boost::make_shared<Client>(ioContextClient, boost::asio::ip::make_address(ip), port);
    client->Start();

    ioContextClient.run_one();
    ioContextServer.run_one();
    ioContextClient.run_one();
    std::string text = stream.str();
    REQUIRE(text == "Connection is UP\nConnected clients: 1\n");

    std::cout.rdbuf(oldStream);
}

TEST_CASE("Connect one more client and they both received the message with \"clients count\"",
    "[client][multiple][message][on-connect][on-send]")
{
    std::stringstream stream;
    std::streambuf* oldStream = std::cout.rdbuf(stream.rdbuf());
    std::string ip = "127.0.0.1";
    int port = 30001;
    boost::asio::io_context ioContextServer;
    auto server = boost::make_shared<Server>(ioContextServer, boost::asio::ip::make_address(ip), port);

    server->Start();
    boost::asio::io_context ioContextClient1;
    auto client1 = boost::make_shared<Client>(ioContextClient1, boost::asio::ip::make_address(ip), port);
    client1->Start();

    ioContextClient1.run_one();
    ioContextServer.run_one();
    ioContextClient1.run_one();
    ioContextServer.run_one();

    boost::asio::io_context ioContextClient2;
    auto client2 = boost::make_shared<Client>(ioContextClient2, boost::asio::ip::make_address(ip), port);
    client2->Start();

    ioContextClient2.run_one();
    ioContextServer.run_one();
    ioContextClient1.run_one();
    ioContextClient2.run_one();

    std::string text = stream.str();
    REQUIRE(text == "Connection is UP\nConnected clients: 1\nConnection is UP\nConnected clients: 2\nConnected clients: 2\n");

    std::cout.rdbuf(oldStream);
}

TEST_CASE("Disconnect one of the client and the other connected clients gets the message with \"clients count\"",
    "[client][single][message][on-disconnect]")
{
    std::stringstream stream;
    std::streambuf* oldStream = std::cout.rdbuf(stream.rdbuf());
    std::string ip = "127.0.0.1";
    int port = 30001;
    boost::asio::io_context ioContextServer;
    auto server = boost::make_shared<Server>(ioContextServer, boost::asio::ip::make_address(ip), port);

    server->Start();
    boost::asio::io_context ioContextClient1;
    auto client1 = boost::make_shared<Client>(ioContextClient1, boost::asio::ip::make_address(ip), port);
    client1->Start();

    ioContextClient1.run_one();
    ioContextServer.run_one();
    ioContextClient1.run_one();
    ioContextServer.run_one();

    {
	    boost::asio::io_context ioContextClient2;
	    auto client2 = boost::make_shared<Client>(ioContextClient2, boost::asio::ip::make_address(ip), port);
	    client2->Start();
    }

    ioContextServer.run_one();
    ioContextClient1.run_one();
    ioContextServer.run_one();
    ioContextServer.run_one();
    ioContextServer.run_one();
    ioContextClient1.run_one();

    std::string text = stream.str();
    REQUIRE(text == "Connection is UP\nConnected clients: 1\nConnected clients: 2\nConnected clients: 1\n");

    std::cout.rdbuf(oldStream);
}
