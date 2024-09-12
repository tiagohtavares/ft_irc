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

# define ENDL std::cout << std::endl;

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
		void	listChannels(Client &client);
		bool	isChannelExist(const std::string &channelName) const;
		bool	isClientInChannel(std::string &channelName, Client &client);




		std::string _cmd;
		std::vector<std::string> _params;

		void	handleNewConnection();
		void	processClientMessage(int clientFd, std::string cmd, std::vector<std::string>params);
		void	cleanup();
		void	cleanupClient(int clientFd);
		void	splitCmdLine(std::string input);
		void 	printParams() const;
		bool 	isNicknameInUse(const std::string &nick) const;



	// --- Commands
		void	pass_cmd(int clientFd, std::vector<std::string> params);
		void	nick_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	user_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	privmsg_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	join_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	topic_cmd(int clientFd, std::vector<std::string> params);
		void	part_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	quit_cmd(int clientFd);

		Client* findClientByNickname(const std::string& nickname, int operatorFd);
		void 	kick_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	names_cmd(int clientFd, std::vector<std::string> params);
		void	msg_cmd(Client &client, int clientFd, std::vector<std::string> params) const;
};
