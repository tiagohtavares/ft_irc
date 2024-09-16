#include "../../includes/Server.hpp"

void	Server::quit_cmd(Client &client)
{
	leaveAllChannels(client);
	cleanupClient(client.getClientFd());
	std::cout << "Received close message. Shutting down client." << std::endl;
}
