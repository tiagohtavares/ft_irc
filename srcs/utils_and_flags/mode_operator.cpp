#include "../../includes/Server.hpp"

/*
	* MODE_OPERATOR
	* 	mode #channel -/+o
	* 	mode #channel -/+o member_target
*/
void Server::mode_operator(Client &client, int clientFd, std::vector<std::string> params)
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

	if (!it->second.isOperator(client))
	{
		sendMessage(clientFd, "482 " + client.getNickName() + " " + params[0] + " :You're not channel operator\n");
		return;
	}

	if (params.size() == 2)
	{
		if (params[1] == "+o")
		{
			it->second.setOperatorMode(true);
			// sendMessage(clientFd, "Only operators can change channel modes now.\n");
		}
		else if (params[1] == "-o")
		{
			it->second.setOperatorMode(false);
			// sendMessage(clientFd, "Any user can change channel modes now.\n");
		}
		else
		{
			sendMessage(clientFd, "472 " + client.getNickName() + " " + params[1] + " :is an unknown mode character\n");
		}
		return;
	}

	if (params.size() == 3)
	{
		if (!it->second.isMember(params[2]))
		{
			sendMessage(clientFd, "401 " + client.getNickName() + " " + params[2] + " :No such nick/channel\n");
			return;
		}

		if (params[1] == "+o")
		{
			if (it->second.isOperator(params[2]))
			{
				sendMessage(clientFd, "433 " + client.getNickName() + " " + params[2] + " :Nickname is already operator\n");
				return;
			}
			it->second.setOperator(params[2]);
			// std::cout << "Added operator: " << params[2] << std::endl;
			// sendMessage(clientFd, params[2] + " is now an operator of " + params[0] + ".\n");
		}
		else if (params[1] == "-o")
		{
			if (it->second.isOperator(params[2]))
			{
				it->second.removeOperator(params[2]);
				// sendMessage(clientFd, params[2] + " is no longer an operator of " + params[0] + "!\n");
				return;
			}
		}
		else
		{
			sendMessage(clientFd, "472 " + client.getNickName() + " " + params[1] + " :is an unknown mode character\n");

		}
		return;
	}
}


// #include "../../includes/Server.hpp"

// void	Server::mode_operator(Client &client, int clientFd, std::vector<std::string> params)
// {
// 	if (params.size() == 2)// MODE #CHANNAL -/+o
// 	{
// 		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
// 		if (it == _channels.end())
// 		{
// 			sendMessage(clientFd, "Error: No such channel.\n");
// 			return;
// 		}
// 		if (!it->second.isOperator(client))
// 		{
// 			sendMessage(clientFd, "Only operators can change operator mode this channel.\n");
// 		}
// 		else if (it->second.isOperator(client) && params[1].compare("+o") == 0)
// 		{
// 			it->second.setOperatorMode(true);
// 			sendMessage(clientFd, "Only operators will can change channel modes.\n");
// 			return ;
// 		}
// 		else if (it->second.isOperator(client) && params[1].compare("-o") == 0)
// 		{
// 			it->second.setOperatorMode(false);
// 			sendMessage(clientFd, "Any user will can change channel modes.\n");
// 			return ;
// 		}
// 		else
// 		{
// 			sendMessage(clientFd, "Error: Invalid mode.\n");
// 		}
// 	}
// 	else if (params.size() == 3)// MODE #CHANNAL -/+o MENBER_NAME
// 	{
// 		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
// 		if (it == _channels.end())
// 		{
// 			sendMessage(clientFd, "Error: No such channel.\n");
// 			return;
// 		}
// 		if (!it->second.isOperator(client))
// 		{
// 			sendMessage(clientFd, "Only operators can give operator status to members of this channel.\n");
// 		}
// 		else if (it->second.isOperator(client) && it->second.isMember(params[2]) && params[1].compare("+o") == 0)
// 		{
// 			it->second.setOperator(params[2]);
// 			sendMessage(clientFd, params[2] + "is operator the this channel now!\n");
// 			return ;
// 		}
// 		else if (it->second.isOperator(client) && params[1].compare("-o") == 0)
// 		{
// 			it->second.removeOperator(params[2]);
// 			sendMessage(clientFd, params[2] + "is no longer the " + params[0] + " operator!\n");
// 			return ;
// 		}
// 		else
// 			sendMessage(clientFd, "Error: Invalid mode.\n");
// 	}
// 	else
// 	{
// 		sendMessage(clientFd, "Error: Incorrect number of parameters for the mode\n");
// 		return;
// 	}
// }
