#include "../../includes/Server.hpp"

void Server::msg_cmd(Client &client, int clientFd, std::vector<std::string> params) const
{
	if (params.size() < 2)
	{
		std::string errorMessage = "Error: Not enough parameters for MSG command. Usage: MSG <channel> <message>\n";
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
			std::map<int, Client>::iterator it;

			for (it = channel.getMembers().begin(); it != channel.getMembers().end(); it++)
			{
				if (it->first != clientFd)
				{
					std::string message = client.getNickName() + ": ";
					for (size_t i = 1; i < params.size(); ++i)
					{
						message += params[i] + ' ';
					}
					message += "\n";
					send(it->first, message.c_str(), message.size(), 0);
				}
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
