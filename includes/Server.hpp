#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>
#include <poll.h>
#include <map>

class Server
{
	public:
		Server(int port, const std::string &password);
		~Server();
		void run();
	private:
		int _port;
		std::string _password;
		int _server_fd;
		sockaddr_in _serverAddress; //server_adress
		std::vector<struct pollfd> _pollfds; // stores pollfd structs representing a fd to monitor
		std::map<int, bool> _authenticatedClients;
		void start();
};
