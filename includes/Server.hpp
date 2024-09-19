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
#include <cerrno>
# include <sstream>
#include <iostream>
#include <csignal>  // For signal handling
#include <cstdlib>  // For exit()
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
		std::map<int, std::string> _clientBuffers;
		void	start();

		// Client management
		std::map<int, Client> _mapClients;


		// Channel management
		std::map<std::string, Channel>	_channels;
		void	createChannel(std::string &channelName, Client &creator);
		void	deleteChannel(std::string &channelName);
		void	listChannels(Client &client);
		bool	isChannelExist(const std::string &channelName) const;
		bool	isClientInChannel(std::string &channelName, Client &client);
		Channel* getChannelByName(const std::string& name);



		std::string _cmd;
		std::vector<std::string> _params;

		void	handleNewConnection();
		void	processClientMessage(int clientFd, std::string cmd, std::vector<std::string>params);
		void	cleanup();
		void	cleanupClient(int clientFd);
		void	splitCmdLine(std::string input);
		void 	printParams() const;
		bool 	isNicknameInUse(const std::string &nick) const;
		void	processClientData(int clientFd);


		// --- Commands
		void	pass_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	nick_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	user_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	privmsg_cmd(Client &client,int clientFd, std::vector<std::string> params);
		void	join_cmd(Client &client, int clientFd, std::vector<std::string> params);
		bool	isChannelNameValid(const std::vector<std::string> &params, int clientFd);
		void	joinChannelWithoutPassword(Client &client, int clientFd, const std::string &channelName);
		void	joinChannelWithPassword(Client &client, int clientFd, const std::string &channelName, const std::string &password);
		void	handleChannelJoin(Client &client, int clientFd, Channel &channel);
		void	putMemberList(Client &client, Channel &channel);


		void	handleExistingChannelJoin(Client &client, int clientFd, std::vector<std::string> &params);
		void	createNewChannel(Client &client, int clientFd, std::vector<std::string> &params);
		void	leaveAllChannels(Client &client);
		void	notifyChannelJoin(const Client &client, int clientFd, Channel &channel);
		void	topic_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	part_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	quit_cmd(Client &client);
		void 	kick_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	names_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	msg_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	invite_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	whois_cmd(Client &client, std::vector<std::string> params);

		Client* findClientByNickname(const std::string &nickname);
		Client * createChannelPassword(const std::string &channelName, Client &client, const std::string &password);

		void	mode_cmd(Client &client, int clientFd, std::vector<std::string> params);
		void	mode_operator(Client &client, int clientFd, std::vector<std::string> params);
		void	mode_topic(Client &client, int clientFd, std::vector<std::string> params);
		void	mode_invite_only(Client &client, int clientFd, std::vector<std::string> params);
		void	mode_password(Client &client, int clientFd, std::vector<std::string> params);
		void	mode_limit(Client &client, int clientFd, std::vector<std::string> params);

		//ultis
		void sendMessage(int fd, const std::string& message);
		void sendWelcomeMessageServe(int fd);
		std::vector<std::string> split(const std::string& str, const std::string& delimiter);
		std::string buildWelcomeMessage(Channel &channel);
		void sendToChannel(const std::string& channelName, const std::string& message);
};
