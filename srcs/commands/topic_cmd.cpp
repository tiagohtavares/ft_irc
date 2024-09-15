#include "../../includes/Server.hpp"

// Função para enviar o tópico do canal para o cliente
void sendChannelTopic(int clientFd, const std::string& channelName, const std::string& topic) 
{
    std::stringstream ss; // Converter clientFd para string usando stringstream
    ss << clientFd; 
    std::string clientFdStr = ss.str();

    // Enviar o tópico do canal usando o código 332
    std::string topicMessage = ":server 332 " + clientFdStr + " " + channelName + " :" + topic + "\r\n";
    send(clientFd, topicMessage.c_str(), topicMessage.size(), 0);

    // Enviar uma mensagem de fim de tópicos (opcional, pode ser enviado após outros tópicos, se aplicável)
    std::string endOfTopicMessage = ":server 333 " + clientFdStr + " " + channelName + " :End of /TOPIC list.\r\n";
    send(clientFd, endOfTopicMessage.c_str(), endOfTopicMessage.size(), 0);
}


void	Server::topic_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	std::string channelName = params[0];
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	// Channel &channel = it->second;

	if (isChannelExist(params.front()))
	{
		if (params.size() == 1)
		{
			sendMessage(clientFd, _channels[params.front()].getTopic() + "\n");
			sendChannelTopic(clientFd, channelName, _channels[params.front()].getTopic());
		} 
		else if (it->second.isOperator(client) || it->second.getTopicMode() == false)
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
				sendChannelTopic(clientFd, channelName, channelName+": "+newTopic);
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