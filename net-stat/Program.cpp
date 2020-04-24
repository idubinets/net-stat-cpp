#include <Server.h>
#include <Client.h>

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

int main(int argc, char* argv[]){
    try {
        std::string ip;
        int port;
        const std::string helpOptionName = "help";
        const std::string serverOptionName = "server";
        const std::string clientOptionName = "client";
        const std::string ipOptionName = "ip";
        const std::string portOptionName = "port";

        boost::program_options::options_description desc("options");
        desc.add_options()
            (helpOptionName.c_str(), "produce help message")
            (serverOptionName.c_str(), "create server")
            (clientOptionName.c_str(), "create client")
            (ipOptionName.c_str(), boost::program_options::value<std::string>(&ip)->default_value("127.0.0.1"), "set ip")
            (portOptionName.c_str(), boost::program_options::value<int>(&port)->default_value(30001), "set port")
            ;
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
		
        if (vm.count(helpOptionName)) {
            std::cout << "Usage: options_description [options]\n" << desc;
            return 0;
        }
        if (vm.count(serverOptionName)) {
            boost::asio::io_context ioContext;
            auto server = boost::make_shared<Server>(ioContext, boost::asio::ip::make_address(ip), port);
            server->Start();
            ioContext.run();
        }
        else {
            if (vm.count(clientOptionName)) {
                boost::asio::io_context ioContext;
                auto client = boost::make_shared<Client>(ioContext, boost::asio::ip::make_address(ip), port);
                client->Start();
                ioContext.run();
            }
            else {
                std::cout << "Neither client nor server were selected" << std::endl;
                return 1;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
