#include "../../includes/Server.hpp"

void Server::msg_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2)
	{
		std::string errorMessage = "Error: Not enough parameters for MSG command. Usage: MSG <channel> <message>\r\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
	std::string channelName = params[0];

	std::map<std::string, Channel>::iterator channelIt = _channels.find(channelName);
	if (channelIt != _channels.end())
	{
		Channel &channel = channelIt->second;
		if (!channel.isMember(client)) // Verificar se o cliente faz parte do canal
		{
			//Se o cliente não é membro (ou foi expulso), enviar mensagem de erro
			std::string errorMessage = ":442 " + client.getNickName() + " " + channelName + " :You're not on that channel\r\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			return;
		}
		else
		{
			//Preparar a mensagem
			std::string message;
			for (size_t i = 1; i < params.size(); ++i)
			{
				message += params[i];
				if (i < params.size() - 1)
				{
					message += ' ';
				}
			}
			message += "\r\n"; // Certificar-se que a mensagem termina corretamente com \r\n
			const std::map<int, Client*> &members = channel.getMembers(); // Enviar a mensagem para todos os membros do canal, exceto o remetente
			for (std::map<int, Client*>::const_iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt)
			{
				if (memberIt->first != clientFd)
				{
					std::string formattedMessage = ":" + client.getNickName() + "!" + client.getUserName() + "@" + "hostname" + " PRIVMSG " + channel.getChannelName() + " :" + message;
					send(memberIt->first, formattedMessage.c_str(), formattedMessage.size(), 0);
				}
			}
		}
	}
	else
	{
		sendMessage(clientFd, ":403 " + channelName + " :No such channel\n"); //Se o canal não existe, enviar mensagem de erro
	}
}