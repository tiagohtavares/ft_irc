// Server.hpp
# include <iostream>
# include <string>
# include <stack>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <sys/select.h>
// # include <stdlib.h>
# include <cstdlib> // substitui a biblioteca stdlib.h
# include <poll.h>
# include <map>

# include <set>

# include "../includes/Client.hpp"

class Server
{
	public:
		Server(int port, const std::string &password);
		~Server();
		void run();

	private:
		int							_port;
		std::string					_password;
		int							_server_fd;
		sockaddr_in					_serverAddress; //server_adress
		std::vector<struct pollfd>	_pollfds; // stores pollfd structs representing a fd to monitor
		std::map<int, bool>			_authenticatedClients;

		void	start();

		std::map<int, Client> _mapClients;
		std::string _cmd;
		std::stack<std::string> _params;

		void	handleNewConnection();
		void	processClientMessage(int clientFd, const std::string &receivedMessage);
		void	cleanup();
		void	cleanupClient(int clientFd);
		void	splitCmdLine(std::string input);
};
