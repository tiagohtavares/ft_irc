
#include "../../includes/Server.hpp"

void Server::mode_topic(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() != 2)
	{
		sendMessage(clientFd, "461 " + client.getNickName() + " MODE :Not enough parameters\n");
		return;
	}

	std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
	if (it == _channels.end())
	{
		sendMessage(clientFd, "403 " + client.getNickName() + " " + params[0] + " :No such channel\n");
		return;
	}

	if (!it->second.isMember(client))
	{
		sendMessage(clientFd, "442 " + client.getNickName() + " " + params[0] + " :You're not on that channel\n");
		return;
	}

	bool isValidMode = (params[1] == "+t" || params[1] == "-t");
	if (!isValidMode)
	{
		sendMessage(clientFd, "472 " + client.getNickName() + " " + params[1] + " :is an unknown mode character\n");
		return;
	}

	bool isOpModeAndOpClient = it->second.getOperatorMode() && it->second.isOperator(client);

	if (params[1] == "+t" && (!it->second.getOperatorMode() || isOpModeAndOpClient))
	{
		it->second.setTopicMode(true);
		sendMessage(clientFd, "Only operators can change the channel topic.\n");
	}
	else if (params[1] == "-t" && (!it->second.getOperatorMode() || isOpModeAndOpClient))
	{
		it->second.setTopicMode(false);
		sendMessage(clientFd, "Any members can change the channel topic.\n");
	}
	else
	{
		sendMessage(clientFd, "482 " + client.getNickName() + " " + params[0] + " :You're not channel operator\n");
	}
}


// void Server::mode_topic(Client &client, int clientFd, std::vector<std::string> params)
// {
// 	// /MODE <channel_name> +/-t
// 	if (params.size() != 2)
// 	{
// 		sendMessage(clientFd, "Error: Not enough parameters for MODE command. Usage: /MODE <channel_name> +/-i \n");
// 		return;
// 	}
// 	// std::string channelName = params[0];
// 	std::map<std::string, Channel>::iterator it = _channels.find(params[0]);

// 	if (it == _channels.end())
// 	{
// 		sendMessage(clientFd, "Error: No such channel.\n");
// 		return;
// 	}
// 	if (it->second.isMember(client))
// 	{
// 		if (it->second.getOperatorMode() == false && params[1].compare("+t") == 0)
// 		{
// 			it->second.setTopicMode(true);
// 			sendMessage(clientFd, "Only operators can change the channel topic.\n");
// 			return ;
// 		}
// 		else if (it->second.getOperatorMode() == false && params[1].compare("-t") == 0)
// 		{
// 			it->second.setTopicMode(false);
// 			sendMessage(clientFd, "Any members can change the channel topic.\n");
// 			return ;
// 		}
// 		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+t") == 0)
// 		{
// 			it->second.setTopicMode(true);
// 			sendMessage(clientFd, "Only operators can change the channel topic.\n");
// 			return ;
// 		}
// 		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-t") == 0)
// 		{
// 			it->second.setTopicMode(false);
// 			sendMessage(clientFd, "Any members can change the channel topic.\n");
// 			return ;
// 		}
// 		else
// 		{
// 			sendMessage(clientFd, "Error: Invalid mode. Supported mode: +/-i\n");
// 		}
// 	}
// 	else
// 		sendMessage(clientFd, "Error: You are not member this channel.\n");
// }
