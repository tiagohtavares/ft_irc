
#include "../../includes/Server.hpp"

void Server::mode_topic(Client &client, int clientFd, std::vector<std::string> params)
{
	// /MODE <channel_name> +/-t
	if (params.size() != 2)
	{
		sendMessage(clientFd, "Error: Not enough parameters for MODE command. Usage: /MODE <channel_name> +/-i \n");
		return;
	}
	// std::string channelName = params[0];
	std::map<std::string, Channel>::iterator it = _channels.find(params[0]);

	if (it == _channels.end())
	{
		sendMessage(clientFd, "Error: No such channel.\n");
		return;
	}
	if (it->second.isMember(client))
	{
		if (it->second.getOperatorMode() == false && params[1].compare("+t") == 0)
		{
			it->second.setTopicMode(true);
			sendMessage(clientFd, "Only operators can change the channel topic.\n");
			return ;
		}
		else if (it->second.getOperatorMode() == false && params[1].compare("-t") == 0)
		{
			it->second.setTopicMode(false);
			sendMessage(clientFd, "Any members can change the channel topic.\n");
			return ;
		}
		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+t") == 0)
		{
			it->second.setTopicMode(true);
			sendMessage(clientFd, "Only operators can change the channel topic.\n");
			return ;
		}
		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-t") == 0)
		{
			it->second.setTopicMode(false);
			sendMessage(clientFd, "Any members can change the channel topic.\n");
			return ;
		}
		else
		{
			sendMessage(clientFd, "Error: Invalid mode. Supported mode: +/-i\n");
		}
	}
	else
		sendMessage(clientFd, "Error: You are not member this channel.\n");
}