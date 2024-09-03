#include "../../includes/Server.hpp"

void	Server::part_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 1)
	{
		if (isChannelExist(params.front()))
		{
			if (isClientInChannel(params.front(), client))
			{
				_channels[params.front()].removeMember(client);
			}
			else
			{
				const std::string errorMessage = "You are not in the " + params.front() + " channel.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			}
		}
		else
		{
			const std::string errorMessage = "The " + params.front() + " channel does not exist.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
		params.front();
	}
	else
	{
		const std::string errorMessage = "Invalid PART command. Connection will be closed.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		cleanupClient(clientFd);
		return;
	}
}
