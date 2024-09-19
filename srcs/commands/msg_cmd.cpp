#include "../../includes/Server.hpp"

void Server::msg_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2)
	{
		std::string errorMessage = "Error: Not enough parameters for MSG command. Usage: MSG <channel> <message>\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}

	std::string nickRecipient = params[0];
	std::map<int, Client>::iterator it = _mapClients.begin();
	for (; it != _mapClients.end(); ++it)
	{
		if (it->second.getNickName() == nickRecipient)
		{
			break;
		}
	}

	if (it != _mapClients.end())
	{
		std::string message;
		for (size_t i = 1; i < params.size(); ++i)
		{
			message += params[i];
			if (i < params.size() - 1)
			{
				message += ' ';
			}
		}
		message += "\r\n";
		std::string formattedMessage = ":" + client.getNickName() + " PRIVMSG " + nickRecipient + " :" + message;
		send(it->first, formattedMessage.c_str(), formattedMessage.size(), 0);
	}
	else
	{
		std::string errorMessage = ":401 " + client.getNickName() + " " + nickRecipient + " :No such nick/channel\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
	}
}
