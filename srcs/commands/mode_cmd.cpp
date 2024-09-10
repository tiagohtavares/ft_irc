#include "../../includes/Server.hpp"

void	Server::mode_operator(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 2)// MODE #CHANNAL -/+o
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			std::string errorMessage = "Error: No such channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			return;
		}
		if (!it->second.isOperator(client))
		{
			std::string message = "Only operators can change operator mode this channel.\n";
			send(clientFd, message.c_str(), message.size(), 0);
		}
		else if (it->second.isOperator(client) && params[1].compare("+o") == 0)
		{
			it->second.setOperatorMode(true);
			std::string message = "Only operators will can change channel modes.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else if (it->second.isOperator(client) && params[1].compare("-o") == 0)
		{
			it->second.setOperatorMode(false);
			std::string message = "Any user will can change channel modes.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else
		{
			std::string errorMessage = "Error: Invalid mode.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else if (params.size() == 3)// MODE #CHANNAL -/+o MENBER_NAME
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			std::string errorMessage = "Error: No such channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			return;
		}
		if (!it->second.isOperator(client))
		{
			std::string message = "Only operators can give operator status to members of this channel.\n";
			send(clientFd, message.c_str(), message.size(), 0);
		}
		else if (it->second.isOperator(client) && it->second.isMember(params[2]) && params[1].compare("+o") == 0)
		{
			it->second.setOperator(params[2]);
			std::string message = params[2] + "is operator the this channel now!\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else if (it->second.isOperator(client) && params[1].compare("-o") == 0)
		{
			it->second.removeOperator(params[2]);
			std::string message = params[2] + "is no longer the " + params[0] + " operator!\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else
		{
			std::string errorMessage = "Error: Invalid mode.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else
	{
		std::string errorMessage = "Error: Incorrect number of parameters for the mode\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
}

void Server::mode_invite_only(Client &client, int clientFd, std::vector<std::string> params)
{
	// /MODE <channel_name> +/-i
	if (params.size() != 2)
	{
		std::string errorMessage = "Error: Not enough parameters for MODE command. Usage: /MODE <channel_name> +/-i \n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
	// std::string channelName = params[0];
	std::map<std::string, Channel>::iterator it = _channels.find(params[0]);

	if (it == _channels.end())
	{
		std::string errorMessage = "Error: No such channel.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
	if (it->second.isMember(client))
	{
		if (it->second.getOperatorMode() == false && params[1].compare("+i") == 0)
		{
			it->second.setInvitedMode(true);
			std::string message = "Only invited users can join the channel.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else if (it->second.getOperatorMode() == false && params[1].compare("-i") == 0)
		{
			it->second.setInvitedMode(false);
			std::string message = "Any user can join the channel.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+i") == 0)
		{
			it->second.setInvitedMode(true);
			std::string message = "Only invited users can join the channel.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-i") == 0)
		{
			it->second.setInvitedMode(false);
			std::string message = "Any user can join the channel.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else
		{
			std::string errorMessage = "Error: Invalid mode. Supported mode: +/-i\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else
	{
		std::string errorMessage = "Error: You are not member this channel.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
	}
}

void	Server::mode_limit(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 2)// /MODE <channel_name> -l
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			std::string errorMessage = "Error: No such channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			return;
		}
		if (it->second.isMember(client))
		{
			if (it->second.getOperatorMode() == false && params[1].compare("-l") == 0)
			{
				it->second.setLimitMode(false);
				std::string message = "Member limit mode has been disabled.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return ;
			}
			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-l") == 0)
			{
				it->second.setLimitMode(false);
				std::string message = "Member limit mode has been disabled.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return ;
			}
			else
			{
				std::string errorMessage = "Error: Invalid mode.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			}
		}
		else
		{
			std::string errorMessage = "Error: You are not member this channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else if (params.size() == 3)// /MODE <channel_name> +l <qnt>
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			std::string errorMessage = "Error: No such channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			return;
		}
		if (it->second.isMember(client))
		{
			if (it->second.getOperatorMode() == false && params[1].compare("+l") == 0)
			{
				unsigned int limit = std::atoi(params[2].c_str());
				if (limit <= 0)
				{
					std::string errorMessage = "Error: Invalid limit. Limit must be greater than 0.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					return;
				}
				it->second.setLimitMode(true);
				it->second.setLimit(limit);
				std::string message = "Only " + params[2] + " users can join the channel.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return ;
			}
			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+l") == 0)
			{
				unsigned int limit = std::atoi(params[2].c_str());
				if (limit <= 0)
				{
					std::string errorMessage = "Error: Invalid limit. Limit must be greater than 0.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					return;
				}
				it->second.setLimitMode(true);
				it->second.setLimit(limit);
				std::string message = "Only " + params[2] + " users can join the channel.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return ;
			}
			else
			{
				std::string errorMessage = "Error: Invalid mode.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			}
		}
		else
		{
			std::string errorMessage = "Error: You are not member this channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else
	{
		std::string errorMessage = "Error: Incorrect number of parameters for the mode\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
}

void	Server::mode_password(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 2)// /MODE <channel_name> -k
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			std::string errorMessage = "Error: No such channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			return;
		}
		if (it->second.isMember(client))
		{
			if (it->second.getOperatorMode() == false && params[1].compare("-k") == 0)
			{
				it->second.setPasswordMode(false);
				std::string message = "Any user can join the channel without a password.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return ;
			}
			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-k") == 0)
			{
				it->second.setPasswordMode(false);
				std::string message = "Any user can join the channel without a password.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return ;
			}
			else
			{
				std::string errorMessage = "Error: Invalid mode.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			}
		}
		else
		{
			std::string errorMessage = "Error: You are not member this channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else if (params.size() == 3)// //MODE <channel_name> +k <password>
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			std::string errorMessage = "Error: No such channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			return;
		}
		if (it->second.isMember(client))
		{
			if (it->second.getOperatorMode() == false && params[1].compare("+l") == 0)
			{
				it->second.setPasswordMode(true);
				it->second.setPassword(params[2]);
				std::cout << "Password Mde: " << it->second.getPasswordMode() << std::endl;
				std::cout << "Password: " << it->second.getPassword() << std::endl;
				std::string message = "Only users with the correct password can join the channel.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return ;
			}
			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+l") == 0)
			{
				it->second.setPasswordMode(true);
				it->second.setPassword(params[2]);
				std::cout << "Password Mde: " << it->second.getPasswordMode() << std::endl;
				std::cout << "Password: " << it->second.getPassword() << std::endl;
				std::string message = "Only users with the correct password can join the channel.\n";
				send(clientFd, message.c_str(), message.size(), 0);
				return ;
			}
			else
			{
				std::string errorMessage = "Error: Invalid mode.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			}
		}
		else
		{
			std::string errorMessage = "Error: You are not member this channel.\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else
	{
		std::string errorMessage = "Error: Incorrect number of parameters for the mode\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
}

void Server::mode_topic(Client &client, int clientFd, std::vector<std::string> params)
{
	// /MODE <channel_name> +/-t
	if (params.size() != 2)
	{
		std::string errorMessage = "Error: Not enough parameters for MODE command. Usage: /MODE <channel_name> +/-i \n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
	// std::string channelName = params[0];
	std::map<std::string, Channel>::iterator it = _channels.find(params[0]);

	if (it == _channels.end())
	{
		std::string errorMessage = "Error: No such channel.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
	if (it->second.isMember(client))
	{
		if (it->second.getOperatorMode() == false && params[1].compare("+t") == 0)
		{
			it->second.setTopicMode(true);
			std::string message = "Only operators can change the channel topic.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else if (it->second.getOperatorMode() == false && params[1].compare("-t") == 0)
		{
			it->second.setTopicMode(false);
			std::string message = "Any members can change the channel topic.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+t") == 0)
		{
			it->second.setTopicMode(true);
			std::string message = "Only operators can change the channel topic.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-t") == 0)
		{
			it->second.setTopicMode(false);
			std::string message = "Any members can change the channel topic.\n";
			send(clientFd, message.c_str(), message.size(), 0);
			return ;
		}
		else
		{
			std::string errorMessage = "Error: Invalid mode. Supported mode: +/-i\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else
	{
		std::string errorMessage = "Error: You are not member this channel.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
	}
}

// void Server::mode_topic(Client &client, int clientFd, std::vector<std::string> params)
// {
//     // Verifica se há pelo menos 3 parâmetros (nickname, modo e nome do canal)
//     if (params.size() != 3)
//     {
//         std::string errorMessage = "Error: Not enough parameters for MODE command. Usage: /MODE <nickname> +/-t <channel_name>\n";
//         send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
//         return;
//     }

//     std::string channelName = params[2];
//     std::map<std::string, Channel>::iterator it = _channels.find(channelName);

//     // Verifica se o canal existe
//     if (it == _channels.end())
//     {
//         std::string errorMessage = "Error: No such channel.\n";
//         send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
//         return;
//     }

// 	if (params[0] != client.getNickName())
//     {
// 		std::string Message = client.getNickName();
//         send(clientFd, Message.c_str(), Message.size(), 0);
//         std::string errorMessage = "\nError: invalid nickname.\n";
//         send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
//         return;
//     }

// 	Channel &channel = it->second;
// 	if (channel.isOperator(client))
// 	{
// 		// Verifica se o segundo parâmetro é válido e se o modo é 't' (topic)
// 		if (params[1].size() == 2 && params[1][1] == 't')
// 		{
// 			// Ativa o modo de tópico apenas para operadores
// 			if (params[1][0] == '+')
// 			{
// 				std::string message = "Only operators can change the channel topic.\n";
// 				send(clientFd, message.c_str(), message.size(), 0);
// 				return channel.setTopicMode(true);
// 			}
// 			// Desativa o modo de tópico para permitir que qualquer membro altere
// 			else if (params[1][0] == '-')
// 			{
// 				std::string message = "Any member of the channel can change the channel topic.\n";
// 				send(clientFd, message.c_str(), message.size(), 0);
// 				return channel.setTopicMode(false);
// 			}
// 			else
// 			{
// 				std::string errorMessage = "Error: Invalid mode. Supported mode: +/-t\n";
// 				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
// 			}
// 		}
// 	}
// }

void Server::mode_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if(params[1] == "+i" || params[1] == "-i")
		mode_invite_only(client, clientFd, params);
	else if(params[1] == "+l" || params[1] == "-l")
		mode_limit(client, clientFd, params);
	else if(params[1] == "+o" || params[1] == "-o")
		mode_operator(client, clientFd, params);
	else if(params[1] == "+k" || params[1] == "-k")
		mode_password(client, clientFd, params);
	else if(params[1] == "+t" || params[1] == "-t")
		mode_topic(client, clientFd, params);
	else
	{
		std::string errorMessage = "Error: Mode not supported.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
}


// SUMMARY OF MODES:
// 	i: Set/remove Invite-only channel
// 		Command:	/mode #channel -/+i
// 		Command:	/invite nick #channel

// 	l: Set/remove the user limit to channel
// 		Command:	/mode #channel -l
// 		Command:	/mode #channel +l limit_quantity

// 	k: Set/remove the channel key (password)
// 		Command:	/mode #channel -k
// 		Command:	/mode #channel +k password
// 		Command:	/join #channel password

// 	o: Give/take channel operator privileges
// 		Command:	/mode #channel -/+o
// 		Command:	/mode #channel -/+o member

// 	t: Set/remove the restrictions of the TOPIC command to channel operators
// 		Command:	/mode #channel -/+t
// 		Command:	/topic #channel topic_text
