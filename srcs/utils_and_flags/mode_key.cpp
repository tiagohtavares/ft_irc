#include "../../includes/Server.hpp"

void	Server::mode_password(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() == 2)// /MODE <channel_name> -k
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			sendMessage(clientFd, "Error: No such channel.\n");
			return;
		}
		if (it->second.isMember(client))
		{
			if (it->second.getOperatorMode() == false && params[1].compare("-k") == 0)
			{
				it->second.setPasswordMode(false);
				sendMessage(clientFd, "Any user can join the channel without a password.\n");
				return ;
			}
			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("-k") == 0)
			{
				it->second.setPasswordMode(false);
				sendMessage(clientFd, "Any user can join the channel without a password.\n");
				return ;
			}
			else
			{
				sendMessage(clientFd, "Error: Invalid mode.\n");
			}
		}
		else
			sendMessage(clientFd, "Error: You are not member this channel.\n");
	}
	else if (params.size() == 3)// //MODE <channel_name> +k <password>
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it == _channels.end())
		{
			sendMessage(clientFd, "Error: No such channel.\n");
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
				sendMessage(clientFd, "Only users with the correct password can join the channel.\n");
				return ;
			}
			else if (it->second.getOperatorMode() == true && it->second.isOperator(client) && params[1].compare("+l") == 0)
			{
				it->second.setPasswordMode(true);
				it->second.setPassword(params[2]);
				std::cout << "Password Mde: " << it->second.getPasswordMode() << std::endl;
				std::cout << "Password: " << it->second.getPassword() << std::endl;
				sendMessage(clientFd, "Only users with the correct password can join the channel.\n");
				return ;
			}
			else
				sendMessage(clientFd, "Error: Invalid mode.\n");
		}
		else
		{
			sendMessage(clientFd, "Error: You are not member this channel.\n");
		}
	}
	else
	{
		sendMessage(clientFd, "Error: Incorrect number of parameters for the mode\n");
		return;
	}
}