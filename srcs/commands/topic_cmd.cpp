#include "../../includes/Server.hpp"

void	Server::topic_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	std::string channelName = params[0];
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	Channel &channel = it->second;

	if (isChannelExist(params.front()))
	{
		if (params.size() == 1) 
			sendMessage(clientFd, _channels[params.front()].getTopic() + "\n");
		else if (channel.isOperator(client) || channel.getTopicMode() == false)
		{
			if (params.size() == 0) 
				sendMessage(clientFd, "Please choose a new topic for the channel");
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
			sendMessage(clientFd, "Error: Only operators can change the channel topic.\n");
	}
	else
		sendMessage(clientFd, "The channel does not exist.\n");
}

// 3.2.4 Topic message

//       Command: TOPIC
//    Parameters: <channel> [ <topic> ]

//    The TOPIC command is used to change or view the topic of a channel.
//    The topic for channel <channel> is returned if there is no <topic>
//    given.  If the <topic> parameter is present, the topic for that
//    channel will be changed, if this action is allowed for the user
//    requesting it.  If the <topic> parameter is an empty string, the
//    topic for that channel will be removed.

//  Examples:

//    :WiZ!jto@tolsun.oulu.fi TOPIC #test :New topic ; User Wiz setting the
//                                    topic.

//    TOPIC #test :another topic      ; Command to set the topic on #test
//                                    to "another topic".

//    TOPIC #test :                   ; Command to clear the topic on
//                                    #test.

//    TOPIC #test                     ; Command to check the topic for
//                                    #test.