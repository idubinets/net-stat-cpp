#include "Server.h"
#include "Client.h"

#include <iostream>
#include <boost/program_options.hpp>

int main(int argc, char* argv[]){
	try {
		std::string ip;
		int port;

		boost::program_options::options_description desc("options");
		desc.add_options()
			("help", "produce help message")
			("server", "create server")
			("client", "create client")
			("ip", boost::program_options::value<std::string>(&ip)->default_value("127.0.0.1"), "set ip")
			("port", boost::program_options::value<int>(&port)->default_value(30001), "set port")
			;
		boost::program_options::variables_map vm;
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
		boost::program_options::notify(vm);


		if (vm.count("help")) {
			std::cout << "Usage: options_description [options]\n" << desc;
			return 0;
		}

		if (vm.count("server"))
		{
			Server server;
			std::cout << "Server is running..." << std::endl;
		}
		else
			if (vm.count("client"))
			{
				Client client;
				std::cout << "Client is running..." << std::endl;
			}

		std::cout << "ip = " << ip << std::endl;
		std::cout << "port = " << port << std::endl;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << "\n";
		return 1;
	}
	return 0;
}
