
#include "../../includes/Server.hpp"

void Server::mode_invite_only(Client &client, int clientFd, std::vector<std::string> params)
{
	// /MODE <channel_name> +/-i
	if (params.size() != 2)
	{
		sendMessage(clientFd, "Error: Not enough parameters for MODE command. Usage: /MODE <channel_name> +/-i \n");
		return;
	}
	// std::string channelName = params[0];
	std::map<std::string, Channel>::iterator it = _channels.find(params[0]);

	if (it == _channels.end())
	{
		sendMessage(clientFd, "Error: No such channel.\n");
		return;
	}
	if (it->second.isMember(client))
	{
		if (it->second.getOperatorMode() == false && params[1].compare("+i") == 0)
		{
			it->second.setInvitedMode(true);
			sendMessage(clientFd, "Only invited users can join the channel.\n");
			return ;
		}
		else if (it->second.getOperatorMode() == false && params[1].compare("-i") == 0)
		{
			it->second.setInvitedMode(false);
			sendMessage(clientFd, "Any user can join the channel.\n");
			return ;
		}
		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+i") == 0)
		{
			it->second.setInvitedMode(true);
			sendMessage(clientFd, "Only invited users can join the channel.\n");
			return ;
		}
		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-i") == 0)
		{
			it->second.setInvitedMode(false);
			sendMessage(clientFd, "Any user can join the channel.\n");
			return ;
		}
		else
			sendMessage(clientFd, "Error: Invalid mode. Supported mode: +/-i\n");
	}
	else
		sendMessage(clientFd, "Error: You are not member this channel.\n");
}