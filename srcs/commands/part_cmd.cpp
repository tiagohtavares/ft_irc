#include "../../includes/Server.hpp"

void	Server::part_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 0 || params.size() == 1)
	{
		if (isChannelExist(params[0]))
		{
			if (isClientInChannel(params[0], client))
			{
				std::string errorMessage = ":442 " + client.getNickName() + " " + _channels[params[0]].getChannelName() + " :You're not on that channel\n";
				// std::string errorMessage = ":442 You have left channel" + _channels[params[0]].getChannelName() + " (Leaving) \n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);

				if (_channels[params[0]].isCreator(client))
				{
					_channels[params[0]].removeCreator(client);
					_channels[params[0]].removeOperator(client);
					_channels[params[0]].removeMember(client);
					_channels[params[0]].removeInvited(client);
					if (_channels[params[0]].getMembers().size() == 0)
					{
						deleteChannel(params[0]);
						return;
					}
				}
				else if (_channels[params[0]].isOperator(client))
				{
					_channels[params[0]].removeOperator(client);
					_channels[params[0]].removeMember(client);
					_channels[params[0]].removeInvited(client);
				}
				else
				{
					_channels[params[0]].removeMember(client);
					_channels[params[0]].removeInvited(client);
				}
				return;
			}
			else
			{
				if (client.getClientFd() != -1)
				{
					sendMessage(clientFd, "You are not in the " + params[0] + " channel.\n");
				}
			}
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
