#include "../../includes/Server.hpp"

void	Server::topic_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	std::string channelName = params[0];
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	Channel &channel = it->second;

	if (isChannelExist(params.front()))
	{
		if (params.size() == 1) 
		{
			const std::string topicMessage = _channels[params.front()].getTopic() + "\n";
			send(clientFd, topicMessage.c_str(), topicMessage.size(), 0);
		} 

		else if (channel.isOperator(client) || channel.getTopicMode() == false)
		{
			if (params.size() == 0) 
			{
				const std::string topicMessage = "Please choose a new topic for the channel";
				send(clientFd, topicMessage.c_str(), topicMessage.size(), 0);
			}
			else if (params.size() >= 2) 
			{
				std::string newTopic;
				for (size_t i = 1; i < params.size(); ++i) 
				{
					newTopic += params[i] + ' ';
				}
				_channels[params.front()].setTopic(newTopic);
			}
		}
		else
		{
			const std::string Message = "Error: Only operators can change the channel topic.\n";
			send(clientFd, Message.c_str(), Message.size(), 0);
		}
	}
	else
	{
		const std::string Message = "The channel does not exist.\n";
		send(clientFd, Message.c_str(), Message.size(), 0);
	}
}