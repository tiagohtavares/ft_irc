#include "../../includes/Server.hpp"


void	Server::join_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if ((params.size() == 1) && (params.front().size() == 1) && ((params.front()[0] == '#') || (params.front()[0] == '&')))
	{
		const std::string message = "/JOIN <#channel_name>\n";
		send(clientFd, message.c_str(), message.size(), 0);
	}
	else if (params.size() == 1 && params.front().size() > 1 && ((params.front()[0] == '#' && params.front()[1] == '#') ||
	(params.front()[0] == '&' && params.front()[1] == '&')))
	{
		const std::string message = "The channel name cannot start with # or &.\n";
		send(clientFd, message.c_str(), message.size(), 0);
	}
	else if (params.size() == 1 && params.front().size() > 1 && (params.front()[0] == '#' || params.front()[0] == '&'))
	{
		if (params.front().size() > 51)
		{
			const std::string message = "The channel name cannot exceed 50 characters.\n";
			send(clientFd, message.c_str(), message.size(), 0);
		}
		else if (isChannelExist(params.front()))
		{
			std::string channelName = params.front();
			std::map<std::string, Channel>::iterator it = _channels.find(channelName);
			if (it !=_channels.end())
			{
				Channel &channel = it->second;
				if (channel.isMember(client))
				{
					const std::string channelTopic = _channels[params.front()].getTopic() + "\n";
					send(clientFd, channelTopic.c_str(), channelTopic.size(), 0);
				}
				else
				{
					const std::string Message = "Welcome to the channel " + _channels[params.front()].getChannelName() + "\n";
					send(client.getClientFd(), Message.c_str(), Message.size(), 0);
					_channels[params.front()].insertMember(client);
				}
			}
		}
		else
		{
			createChannel(params.front(), client);
		}
		params.pop_back();
	}
	else
	{
		const std::string errorMessage = "Invalid JOIN command. Connection will be closed.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
	}
}
