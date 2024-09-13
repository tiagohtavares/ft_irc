#include "../../includes/Server.hpp"

//!Users que foram kicked do canal podem ainda mandar menssagem
void Server::msg_cmd(Client &client, int clientFd, std::vector<std::string> params) const
{
	if (params.size() < 2)
	{

		std::string errorMessage = "Error: Not enough parameters for MSG command. Usage: MSG <channel> <message>\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}

	std::string channelName = params[0];
	std::map<std::string, Channel>::const_iterator channelIt = _channels.find(channelName);

if (channelIt != _channels.end())
{
	const Channel &channel = channelIt->second;
	const std::map<int, Client*> &members = channel.getMembers();

	for (std::map<int, Client*>::const_iterator memberIt = members.begin();
		memberIt != members.end();
		++memberIt)
	{
			if (memberIt->first != clientFd)
			{
				std::string message =   "[" + channel.getChannelName() + "] " + client.getNickName() + ": ";
				for (size_t i = 1; i < params.size(); ++i)
				{
					message += params[i];
					if (i < params.size() - 1)
					{
						message += ' ';
					}
				}
				message += "\n";
				send(memberIt->first, message.c_str(), message.size(), 0);
			}
	}
}
else
{
	std::string errorMessage = "Error: Channel " + channelName + " does not exist.\n";
	send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
}
}
