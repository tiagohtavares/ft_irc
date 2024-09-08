#include "../../includes/Server.hpp"

std::string Server::buildWelcomeMessage(Channel &channel) 
{
    // Construir a mensagem de boas-vindas
    std::string welcomeMessage = "Welcome to the channel " + channel.getChannelName() + "!\n";

    // Adicionar o tópico do canal
    welcomeMessage += "Channel Topic: " + channel.getTopic() + "\n";

    // Adicionar a lista de membros
    std::string membersList = "Channel Members: ";
    std::map<int, Client*> members = channel.getMembers(); 

    for (std::map<int, Client*>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) 
	{    
        Client* memberClient = memberIt->second;// Encontrar o nome do membro usando o descritor de arquivo
        if (memberClient) {
            membersList += memberClient->getNickName() + ", ";
        }
    }
    membersList += "\n";
    welcomeMessage += membersList;
    return welcomeMessage;
}



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
					_channels[params.front()].insertMember(client);
					std::string msg = buildWelcomeMessage(channel);
					send(clientFd, msg.c_str(), msg.size(), 0);
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




// void	Server::join_cmd(Client &client, int clientFd, std::vector<std::string> params)
// {
// 	if ((params.size() == 1) && (params.front().size() == 1) && ((params.front()[0] == '#') || (params.front()[0] == '&')))
// 	{
// 		const std::string message = "/JOIN <#channel_name>\n";
// 		send(clientFd, message.c_str(), message.size(), 0);
// 	}
// 	else if (params.size() == 1 && params.front().size() > 1 && ((params.front()[0] == '#' && params.front()[1] == '#') ||
// 	(params.front()[0] == '&' && params.front()[1] == '&')))
// 	{
// 		const std::string message = "The channel name cannot start with # or &.\n";
// 		send(clientFd, message.c_str(), message.size(), 0);
// 	}


// 	else if ((params.size() == 1 || params.size() == 2) && params.front().size() > 1 && (params.front()[0] == '#' || params.front()[0] == '&'))
// 	{
// 		std::string channelName = params.front();
		
// 		if (channelName.size() > 51)
// 		{
// 			const std::string message = "The channel name cannot exceed 50 characters.\n";
// 			send(clientFd, message.c_str(), message.size(), 0);
// 			return;
// 		}

// 		if (isChannelExist(channelName) && params.size() == 1) // Entrar SEM senha no canal 
// 		{
// 			std::string channelName = channelName;
// 			std::map<std::string, Channel>::iterator it = _channels.find(channelName);
// 			if (it !=_channels.end())
// 			{
// 				Channel &channel = it->second;
// 				if (channel.isMember(client))
// 				{
// 					const std::string channelTopic = _channels[channelName].getTopic() + "\n";
// 					send(clientFd, channelTopic.c_str(), channelTopic.size(), 0);
// 				}
// 				else
// 				{
// 					std::string teste = _channels[channelName].getPassword();
// 					if (teste == "")
// 					{
// 						std::string msg = buildWelcomeMessage(channel);
// 						send(clientFd, msg.c_str(), msg.size(), 0);
// 						_channels[channelName].insertMember(client);
// 					}
					
// 				}
// 			}
// 		}
// 		else if (isChannelExist(channelName) && params.size() == 2) // Entrar com senha no canal 
// 		{
// 			std::map<std::string, Channel>::iterator it = _channels.find(channelName);
// 			if (it !=_channels.end())
// 			{
// 				Channel &channel = it->second;
// 				std::string ChannelPassword = _channels[channelName].getPassword();
// 				if (params[1]  == ChannelPassword)
// 				{
// 					std::string msg = buildWelcomeMessage(channel);
// 					send(clientFd, msg.c_str(), msg.size(), 0);
// 					_channels[channelName].insertMember(client);
// 				}
// 			}	
// 		}
// 		else
// 		{
// 			//verifica se a senha foi setada
// 			if(params.size() == 2)
// 			{
// 				std::map<std::string, Channel>::iterator it = _channels.find(channelName);
// 				if (it !=_channels.end())
// 				{
// 					_channels[channelName].setPassword(params[1]);
// 					createChannel(channelName, client);
// 				}
// 			}
// 			else if(params.size() == 1) 
// 				createChannel(channelName, client);
// 		}
// 		params.pop_back();
// 	}
// 	else
// 	{
// 		const std::string errorMessage = "Invalid JOIN command. Connection will be closed.\n";
// 		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
// 	}
// }


// 3.2.1 Join message

//       Command: JOIN
//    Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] )
//                / "0"

//    The JOIN command is used by a user to request to start listening to
//    the specific channel.  Servers MUST be able to parse arguments in the
//    form of a list of target, but SHOULD NOT use lists when sending JOIN
//    messages to clients.

//    Once a user has joined a channel, he receives information about
//    all commands his server receives affecting the channel.  This
//    includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.
//    This allows channel members to keep track of the other channel
//    members, as well as channel modes.

//    If a JOIN is successful, the user receives a JOIN message as
//    confirmation and is then sent the channel's topic (using RPL_TOPIC) and
//    the list of users who are on the channel (using RPL_NAMREPLY), which
//    MUST include the user joining.

//    Note that this message accepts a special argument ("0"), which is
//    a special request to leave all channels the user is currently a member
//    of.  The server will process this message as if the user had sent
//    a PART command (See Section 3.2.2) for each channel he is a member
//    of.

//   JOIN #foo,&bar fubar            ; Command to join channel #foo using
//                                    key "fubar" and &bar using no key.

//    JOIN #foo,#bar fubar,foobar     ; Command to join channel #foo using
//                                    key "fubar", and channel #bar using
//                                    key "foobar".

//    JOIN #foo,#bar                  ; Command to join channels #foo and
//                                    #bar.

//    JOIN 0                          ; Leave all currently joined
//                                    channels.

//    :WiZ!jto@tolsun.oulu.fi JOIN #Twilight_zone ; JOIN message from WiZ
//                                    on channel #Twilight_zone
