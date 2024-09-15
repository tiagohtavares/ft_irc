#include "../../includes/Server.hpp"

void	Server::part_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 2)
	{
		if (isChannelExist(params[0]))
		{
			if (isClientInChannel(params[0], client))
			{
				if (_channels[params[0]].isCreator(client))
				{
					_channels[params[0]].removeCreator(client);
					_channels[params[0]].removeOperator(client);
					_channels[params[0]].removeMember(client);
					if (_channels[params[0]].getMembers().size() == 0)
					{
						deleteChannel(params[0]);
						return;
					}
					_channels[params[0]].setCreator(*_channels[params[0]].getMembers().begin()->second);
					_channels[params[0]].setOperator(*_channels[params[0]].getMembers().begin()->second);

					std::string message = "The " + _channels[params[0]].getMembers().begin()->second->getNickName() + " has inherited channel creator status.\n";

					_channels[params[0]].sendMessageToMembers(message);
				}
				else if (_channels[params[0]].isOperator(client))
				{
					_channels[params[0]].removeOperator(client);
					_channels[params[0]].removeMember(client);
				}
				else
					_channels[params[0]].removeMember(client);
				std::string message = "You left the " + _channels[params[0]].getChannelName() + " channel.\n";
				send(client.getClientFd(), message.c_str(), message.size(), 0);
			}
			else
				sendMessage(clientFd, "You are not in the " + params[0] + " channel.\n");
		}
		else
			sendMessage(clientFd, "The " + params[0] + " channel does not exist.\n");
		params[0];
	}
	else
	{
		sendMessage(clientFd, "Invalid PART command. Connection will be closed.\n");
		cleanupClient(clientFd);
		return;
	}
}
