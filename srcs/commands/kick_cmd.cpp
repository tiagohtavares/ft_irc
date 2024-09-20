#include "../../includes/Server.hpp"


void Server::sendToChannel(const std::string& channelName, const std::string& message)
{
    // Verifica se o canal existe
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);
    if (it != _channels.end())
    {
        Channel& channel = it->second;
        std::map<int, Client*> members = channel.getMembers();

        // Envia a mensagem para todos os membros do canal
        for (std::map<int, Client*>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt)
        {
            Client* client = memberIt->second;
            if (client)
            {
                int clientFd = client->getClientFd();
                sendMessage(clientFd, message);
            }
        }
    }
    else
    {
        std::cerr << "Channel " << channelName << " does not exist." << std::endl;
        // Se necessário, envie uma mensagem de erro para o cliente que enviou o comando
    }
}


std::vector<std::string> filterPortStrings(std::vector<std::string> params) {
    std::vector<std::string> filteredParams;

    for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); ++it) {
        const std::string& param = *it;
        if (param.find("port_") != 0) { // Checks if the string does not start with "port_"
            filteredParams.push_back(param);
        }
    }

    return filteredParams;
}


void Server::kick_cmd(Client &client, int clientFd, std::vector<std::string> params)
{

    if (params.size() != 2)
    {
        sendMessage(clientFd, "461 KICK :Not enough parameters\r\n");
        return;
    }
	// EXECUTADO NO CANAL: KICK NICKNAME
	// CMD: KICK
	// PARAMS: #canal
	// PARAMS: nickname
    std::string channelName = params[0];
    std::string nickName = params[1];

    if (isChannelExist(channelName))
    {
        std::map<std::string, Channel>::iterator it = _channels.find(channelName);
        if (it != _channels.end())
        {
            Channel& channel = it->second;
            if (channel.isOperator(client))
            {
				if (channel.isCreator(nickName))
				{
					sendMessage(clientFd, channelName + " :You can't kick the creator of the channel\r\n");
				}
                else if (channel.isMember(nickName))
                {
					// Remove o membro do canal
                    channel.removeMember(nickName);

					std::map<int, Client*> itNickname = channel.getMembers();
					for (std::map<int, Client*>::iterator it = itNickname.begin(); it != itNickname.end(); ++it)
					{
						if (it->second->getNickName() == nickName)
						{
							int clientFd = it->second->getClientFd();
							sendMessage(clientFd, "KICK " + channelName + " " + nickName + "\r\n");
						}
					}

					if (channel.isOperator(nickName))
						channel.removeOperator(nickName);
                    // Envia a mensagem KICK para todos os membros do canal
                    std::string kickMessage = ":" + client.getNickName() + " KICK " + channelName + " " + nickName;
                    // if (!comment.empty())
                    //     kickMessage += " :" + comment;
                    kickMessage += "\r\n";
                    sendToChannel(channelName, kickMessage);
                    // Notifica o cliente que executou o comando
                    sendMessage(clientFd, "You have kicked " + nickName + " from channel " + channelName + ".\r\n");
                }
                else
                {
                    sendMessage(clientFd, "441 " + nickName + " " + channelName + " :They aren't on that channel\r\n");
                }
            }
            else
            {
                sendMessage(clientFd, "482 " + channelName + " :You're not channel operator\r\n");
            }
        }
    }
    else
    {
        sendMessage(clientFd, "403 " + channelName + " :No such channel\r\n");
    }
}