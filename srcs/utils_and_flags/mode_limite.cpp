#include "../../includes/Server.hpp"

void	Server::mode_limit(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 2)// /MODE <channel_name> -l
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			sendMessage(clientFd, "Error: No such channel.\n");
			return;
		}
		if (it->second.isMember(client))
		{
			if (it->second.getOperatorMode() == false && params[1].compare("-l") == 0)
			{
				it->second.setLimitMode(false);
				sendMessage(clientFd, "Member limit mode has been disabled.\n");
				return ;
			}
			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-l") == 0)
			{
				it->second.setLimitMode(false);
				sendMessage(clientFd, "Member limit mode has been disabled.\n");
				return ;
			}
			else
			{
				sendMessage(clientFd, "Error: Invalid mode.\n");
			}
		}
		else
		{
			sendMessage(clientFd, "Error: You are not member this channel.\n");
		}
	}
	else if (params.size() == 3)// /MODE <channel_name> +l <qnt>
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			sendMessage(clientFd, "Error: No such channel.\n");
			return;
		}
		if (it->second.isMember(client))
		{
			if (it->second.getOperatorMode() == false && params[1].compare("+l") == 0)
			{
				unsigned int limit = std::atoi(params[2].c_str());
				if (limit <= 0)
				{
					sendMessage(clientFd, "Error: Invalid limit. Limit must be greater than 0.\n");
					return;
				}
				it->second.setLimitMode(true);
				it->second.setLimit(limit);
				sendMessage(clientFd, "Only " + params[2] + " users can join the channel.\n");
				return ;
			}
			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+l") == 0)
			{
				unsigned int limit = std::atoi(params[2].c_str());
				if (limit <= 0)
				{
					sendMessage(clientFd, "Error: Invalid limit. Limit must be greater than 0.\n");
					return;
				}
				it->second.setLimitMode(true);
				it->second.setLimit(limit);
				sendMessage(clientFd, "Only " + params[2] + " users can join the channel.\n");
				return ;
			}
			else
				sendMessage(clientFd, "Error: Invalid mode.\n");
		}
		else
		{
			sendMessage(clientFd, "Error: You are not member this channel.\n");
		}
	}
	else
	{
		sendMessage(clientFd, "Error: Incorrect number of parameters for the mode\n");
		return;
	}
}
