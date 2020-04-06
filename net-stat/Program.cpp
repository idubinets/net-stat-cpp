#include "Server.h"
#include "Client.h"

#include <iostream>

int main(){
	std::cout << "Hello!" << std::endl;
	Server *server = new Server();
	Client *client = new Client();
	delete server;
	delete client;
	std::cout << "Bye!" << std::endl;
	return 0;
}
