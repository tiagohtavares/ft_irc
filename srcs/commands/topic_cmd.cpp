#include "../../includes/Server.hpp"

void	Server::topic_cmd(int clientFd, std::vector<std::string> params)
{
	if (params.size() == 0) 
	{
		const std::string topicMessage = "Please choose a new topic for the channel";
		send(clientFd, topicMessage.c_str(), topicMessage.size(), 0);
	}
	else if (params.size() == 1) 
	{
		const std::string topicMessage = _channels[params.front()].getTopic() + "\n";
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
