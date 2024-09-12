#include "../../includes/Server.hpp"

void	Server::part_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 1)
	{
		if (isChannelExist(params.front()))
		{
			if (isClientInChannel(params.front(), client))
				_channels[params.front()].removeMember(client);
			else
				sendMessage(clientFd, "You are not in the " + params.front() + " channel.\n");
		}
		else
			sendMessage(clientFd, "The " + params.front() + " channel does not exist.\n");
		params.front();
	}
	else
	{
		sendMessage(clientFd, "Invalid PART command. Connection will be closed.\n");
		cleanupClient(clientFd);
		return;
	}
}