#include "../../includes/Server.hpp"

void Server::kick_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2)
	{
		std::string errorMessage = "Error: Not enough parameters for KICK command. Usage: KICK <channel> <user>\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}

	if (isChannelExist(params[0]))
	{
		std::string channelName = params.front();
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		if (it !=_channels.end())
		{
			Channel &channel = it->second;
			if (channel.isOperator(client))
			{
				if (channel.isMember(params[1]))
				{
					channel.removeMember(params[1]);
					std::string kickMessage3 = "You have kicked " + params[1] + " from channel " + channelName + ".\n";
					send(clientFd, kickMessage3.c_str(), kickMessage3.size(), 0);
				}
				else
				{
					std::string errorMessage = "Error: The user " + params[1] + " is not in this channel.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					return;
				}
			}
			else
			{
				std::string errorMessage = "Error: You do not have permission to kick users from this channel.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
				return;
			}
		}
	}
	else
	{
		std::string errorMessage = "Error: Channel " + params[0] + " does not exist.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
}
