#include "../../includes/Server.hpp"

//!Users que foram kicked do canal podem ainda mandar menssagem
void Server::msg_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2)
	{
		std::string errorMessage = "Error: Not enough parameters for MSG command. Usage: MSG <channel> <message>\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
	// std::string nickName = params[1];
	std::string channelName = params[0];

	if (channelName[0] == '#' || channelName[0] == '&' )  // Check if the recipient is a channel
	{
		// Verify if the channel exists
		std::map<std::string, Channel>::iterator channelIt = _channels.find(channelName);
		if (channelIt != _channels.end())
		{
			const Channel &channel = channelIt->second;
			const std::map<int, Client*> &members = channel.getMembers();

			// Prepare the message
			std::string message;
			for (size_t i = 1; i < params.size(); ++i)
			{
				message += params[i];
				if (i < params.size() - 1)
				{
					message += ' ';
				}
			}
			message += "\n";
			// Send the message to all members of the channel
			for (std::map<int, Client*>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt)
			{
				// Exclude the sender from receiving their own message
				if (memberIt->first != clientFd)
				{
					std::string formattedMessage = "[" + channel.getChannelName() + "] " + client.getNickName() + ": " + message;
                	send(memberIt->first, formattedMessage.c_str(), formattedMessage.size(), 0);
				}
			}
		}
		else
		{
			sendMessage(clientFd, ":403 " + channelName + " :No such channel\n");
		}
	}
}