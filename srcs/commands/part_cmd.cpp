#include "../../includes/Server.hpp"

void	Server::part_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 2)
	{
		if (isChannelExist(params[0]))
		{
			if (isClientInChannel(params[0], client))
			{
				_channels[params[0]].removeMember(client);
				if (_channels[params[0]].getMembers().size() == 0)
				{
					deleteChannel(params[0]);
					return;
				}
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
