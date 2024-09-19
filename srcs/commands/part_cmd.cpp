#include "../../includes/Server.hpp"

void	Server::part_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() <= 2)
	{
		if (isChannelExist(params[0]))
		{
			std::string channelName = params[0];
			std::string nickname = client.getNickName();
			if (isClientInChannel(params[0], client))
			{
				if (_channels[params[0]].isCreator(client))
				{
					_channels[params[0]].removeCreator(client);
					_channels[params[0]].removeOperator(client);
					_channels[params[0]].removeMember(client);
					_channels[params[0]].removeInvited(client);
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
				if (isChannelExist(params[0]) && !_channels[params[0]].isMember(client))
				{
					std::string errorMessage = ":442 " + nickname + " " + channelName + " :You're not on that channel\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					return;
				}
				if (_channels[params[0]].getMembers().size() == 0)
				{
					deleteChannel(params[0]);
					return;
				}
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
