#include "../../includes/Server.hpp"

void Server::mode_topic(Client &client, int clientFd, std::vector<std::string> params)
{
    // Verifica se há pelo menos 3 parâmetros (nickname, modo e nome do canal)
    if (params.size() != 3)
    {
        std::string errorMessage = "Error: Not enough parameters for MODE command. Usage: /MODE <nickname> +/-t <channel_name>\n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
    }

    std::string channelName = params[2];
    std::map<std::string, Channel>::iterator it = _channels.find(channelName);

    // Verifica se o canal existe
    if (it == _channels.end())
    {
        std::string errorMessage = "Error: No such channel.\n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
    }

	if (params[0] != client.getNickName())
    {
		std::string Message = client.getNickName();
        send(clientFd, Message.c_str(), Message.size(), 0);
        std::string errorMessage = "\nError: invalid nickname.\n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
    }

	Channel &channel = it->second;
	if (channel.isOperator(client))
	{
		// Verifica se o segundo parâmetro é válido e se o modo é 't' (topic)
		if (params[1].size() == 2 && params[1][1] == 't')
		{
			// Ativa o modo de tópico apenas para operadores
			if (params[1][0] == '+')
			{
				std::string message = "Only operators can change the channel topic.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return channel.setTopicMode(true);
			}
			// Desativa o modo de tópico para permitir que qualquer membro altere
			else if (params[1][0] == '-')
			{
				std::string message = "Any member of the channel can change the channel topic.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return channel.setTopicMode(false);
			}
			else
			{
				std::string errorMessage = "Error: Invalid mode. Supported mode: +/-t\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			}
		}
	}
}

void Server::mode_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() != 3)
	{
		std::string errorMessage = "Error: Not enough parameters for MODE command. Usage: /MODE <your_nickname> +/-<mode> #channel \n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
	if (params[0] == client.getNickName())
	{
		if(params[1] == "+t" || params[1] == "-t") 
			mode_topic(client, clientFd, params);
	}
	else
	{
		std::string errorMessage = "Error: You can only change your own mode.\n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
	}
}
