#include "../../includes/Server.hpp"

void mode_invisible(Client &client, int clientFd, std::vector<std::string> params)
{
	if ((params[1].size() == 2) && params[1][1] == 'i')
	{
		{
			if (!client.getModeInvisible() && params[1][0] == '+')// Ativa o modo invisível
			{
				std::string errorMessage = "You are invisible!\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
				return client.setModeInvisible(true);
			}  
				
			else if (client.getModeInvisible() && params[1][0] == '-')// Desativa o modo invisível
			{	
				std::string errorMessage = "You not are invisible\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
				return client.setModeInvisible(false);	
			} 
			else
			{
				std::string errorMessage = "Error: Invalid mode. Supported mode: +/-i (invisible)\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
			}
		}
	}
}

// void mode_operator(Client &client, int clientFd, std::vector<std::string> params)
// {
// 	if ((params[1].size() == 2) && params[1][1] == 'o')
// 	{
// 		{
// 			if (!client.setOperatorMode() && params[1][0] == '+')// Ativa o modo invisível
// 			{
// 				std::string errorMessage = "You are invisible!\n";
// 				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
// 				return client.setModeInvisible(true);
// 			}  
				
// 			else if (client.getModeInvisible() && params[1][0] == '-')// Desativa o modo invisível
// 			{	
// 				std::string errorMessage = "You not are invisible\n";
// 				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
// 				return client.setModeInvisible(false);	
// 			} 
// 			else
// 			{
// 				std::string errorMessage = "Error: Invalid mode. Supported mode: +/-i (invisible)\n";
// 				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
// 			}
// 		}
// 	}
// }



void Server::mode_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() != 2)
	{
		std::string errorMessage = "Error: Not enough parameters for MODE command. Usage: /MODE <your_nickname> +/-<mode> \n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}
	if (params[0] == client.getNickName())
	{
		if(params[1] == "+i" || params[1] == "-i") //invisible mode
			mode_invisible(client, clientFd, params);
		// else if(params[1] == "+o" || params[1] == "-o") //invisible mode
		// 	mode_operator(client, clientFd, params);
	}
	else
	{
		std::string errorMessage = "Error: You can only change your own mode.\n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
	}
}
