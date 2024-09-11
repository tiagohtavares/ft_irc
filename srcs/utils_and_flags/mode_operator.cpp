#include "../../includes/Server.hpp"

void	Server::mode_operator(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 2)// MODE #CHANNAL -/+o
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			sendMessage(clientFd, "Error: No such channel.\n");
			return;
		}
		if (!it->second.isOperator(client))
		{
			sendMessage(clientFd, "Only operators can change operator mode this channel.\n");
		}
		else if (it->second.isOperator(client) && params[1].compare("+o") == 0)
		{
			it->second.setOperatorMode(true);
			sendMessage(clientFd, "Only operators will can change channel modes.\n");
			return ;
		}
		else if (it->second.isOperator(client) && params[1].compare("-o") == 0)
		{
			it->second.setOperatorMode(false);
			sendMessage(clientFd, "Any user will can change channel modes.\n");
			return ;
		}
		else
		{
			sendMessage(clientFd, "Error: Invalid mode.\n");
		}
	}
	else if (params.size() == 3)// MODE #CHANNAL -/+o MENBER_NAME
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			sendMessage(clientFd, "Error: No such channel.\n");
			return;
		}
		if (!it->second.isOperator(client))
		{
			sendMessage(clientFd, "Only operators can give operator status to members of this channel.\n");
		}
		else if (it->second.isOperator(client) && it->second.isMember(params[2]) && params[1].compare("+o") == 0)
		{
			sendMessage(clientFd, params[2] + "is operator the this channel now!\n");
			return ;
		}
		else if (it->second.isOperator(client) && params[1].compare("-o") == 0)
		{
			it->second.removeOperator(params[2]);
			sendMessage(clientFd, params[2] + "is no longer the " + params[0] + " operator!\n");
			return ;
		}
		else
			sendMessage(clientFd, "Error: Invalid mode.\n");
	}
	else
	{
		sendMessage(clientFd, "Error: Incorrect number of parameters for the mode\n");
		return;
	}
}