# include <iostream>
# include <string>
# include <stack>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <sys/select.h>
# include <cstdlib>
# include <poll.h>
# include <map>
#include <queue>

# include <set>

# include "../includes/Client.hpp"
# include "../includes/Channel.hpp"

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
		sockaddr_in					_serverAddress;
		std::vector<struct pollfd>	_pollfds;
		std::map<int, bool>			_authenticatedClients;

		void	start();

		std::map<int, Client> _mapClients;

		// Channel management
		std::map<std::string, Channel>	_channels;
		void	createChannel(std::string &channelName, Client &creator);
		void	deleteChannel(std::string &channelName);
		void	joinChannel(Client &client, std::string &channelName);
		void	partChannel(Client &client, std::string &channelName);
		void	listChannels(Client &client);

		std::string _cmd;
		std::stack<std::string> _params;

		void	handleNewConnection();
		void	processClientMessage(int clientFd, std::string cmd, std::stack<std::string>	params);
		void	cleanup();
		void	cleanupClient(int clientFd);
		void	splitCmdLine(std::string input);
		void	parse(std::string receivedMessage);
};
