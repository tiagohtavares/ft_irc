#include "../../includes/Server.hpp"

void	Server::mode_password(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2 || params.size() > 3)
	{
		sendMessage(clientFd, "461 " + client.getNickName() + " MODE :Not enough parameters\n");
		return;
	}

	std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
	if (it == _channels.end())
	{
		sendMessage(clientFd, "403 " + client.getNickName() + " " + params[0] + " :No such channel\n");
		return;
	}

	if (!it->second.isMember(client))
	{
		sendMessage(clientFd, "442 " + client.getNickName() + " " + params[0] + " :You're not on that channel\n");
		return;
	}

	bool isValidMode = (params[1] == "-k" || params[1] == "+k");
	if (!isValidMode)
	{
		sendMessage(clientFd, "472 " + client.getNickName() + " " + params[1] + " :is an unknown mode character\n");
		return;
	}

	bool isOpModeAndOpClient = it->second.getOperatorMode() && it->second.isOperator(client);

	if (params[1] == "-k" && (!it->second.getOperatorMode() || isOpModeAndOpClient))
	{
		it->second.setPasswordMode(false, "");
		// sendMessage(clientFd, "Any user can join the channel without a password.\n");
		return;
	}

	if (params.size() == 3 && params[1] == "+k" && (!it->second.getOperatorMode() || isOpModeAndOpClient))
	{
		it->second.setPasswordMode(true, params[2]);
		// std::cout << "Password Mode: " << it->second.getPasswordMode() << std::endl;
		// std::cout << "Password: " << it->second.getPassword() << std::endl;
		// sendMessage(clientFd, "Only users with the correct password can join the channel.\n");
		return;
	}

	if (it->second.getOperatorMode() && !isOpModeAndOpClient)
	{
		sendMessage(clientFd, "482 " + client.getNickName() + " " + params[0] + " :You're not channel operator\n");
		return;
	}

	sendMessage(clientFd, "472 " + client.getNickName() + " " + params[1] + " :Invalid mode or insufficient privileges\n");
}



// void	Server::mode_password(Client &client, int clientFd, std::vector<std::string> params)
// {
// 	if (params.size() == 2)// /MODE <channel_name> -k
// 	{
// 		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
// 		if (it == _channels.end())
// 		{
// 			sendMessage(clientFd, "Error: No such channel.\n");
// 			return;
// 		}
// 		if (it->second.isMember(client))
// 		{
// 			if (it->second.getOperatorMode() == false && params[1].compare("-k") == 0)
// 			{
// 				it->second.setPasswordMode(false);
// 				sendMessage(clientFd, "Any user can join the channel without a password.\n");
// 				return ;
// 			}
// 			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-k") == 0)
// 			{
// 				it->second.setPasswordMode(false);
// 				sendMessage(clientFd, "Any user can join the channel without a password.\n");
// 				return ;
// 			}
// 			else
// 			{
// 				sendMessage(clientFd, "Error: Invalid mode.\n");
// 			}
// 		}
// 		else
// 			sendMessage(clientFd, "Error: You are not member this channel.\n");
// 	}
// 	else if (params.size() == 3)// //MODE <channel_name> +k <password>
// 	{
// 		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
// 		if (it == _channels.end())
// 		{
// 			sendMessage(clientFd, "Error: No such channel.\n");
// 			return;
// 		}
// 		if (it->second.isMember(client))
// 		{
// 			if (it->second.getOperatorMode() == false && params[1].compare("+k") == 0)
// 			{
// 				it->second.setPasswordMode(true);
// 				it->second.setPassword(params[2]);
// 				std::cout << "Password Mde: " << it->second.getPasswordMode() << std::endl;
// 				std::cout << "Password: " << it->second.getPassword() << std::endl;
// 				sendMessage(clientFd, "Only users with the correct password can join the channel.\n");
// 				return ;
// 			}
// 			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+k") == 0)
// 			{
// 				it->second.setPasswordMode(true);
// 				it->second.setPassword(params[2]);
// 				std::cout << "Password Mde: " << it->second.getPasswordMode() << std::endl;
// 				std::cout << "Password: " << it->second.getPassword() << std::endl;
// 				sendMessage(clientFd, "Only users with the correct password can join the channel.\n");
// 				return ;
// 			}
// 			else
// 				sendMessage(clientFd, "Error: Invalid mode.\n");
// 		}
// 		else
// 		{
// 			sendMessage(clientFd, "Error: You are not member this channel.\n");
// 		}
// 	}
// 	else
// 	{
// 		sendMessage(clientFd, "Error: Incorrect number of parameters for the mode\n");
// 		return;
// 	}
// }
