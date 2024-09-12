#include "../../includes/Server.hpp"

void	Server::quit_cmd(int clientFd)
{
	cleanupClient(clientFd);
	std::cout << "Received close message. Shutting down client." << std::endl;
}
