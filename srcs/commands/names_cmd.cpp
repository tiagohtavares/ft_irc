#include "../../includes/Server.hpp"

void	Server::names_cmd(int clientFd, std::vector<std::string> params) 
{
	if (params.size() == 1)
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it != _channels.end())
		{
			_channels[params[0]].memberList(clientFd);
		}
		else
		{
			const std::string errorMessage = "403 " + params[0] + " :No such channel\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else
	{
		const std::string errorMessage = "461 NAMES :Not enough parameters\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
	}
}
