#include "../../includes/Server.hpp"

void	Server::privmsg_cmd(int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2)
	{
		const std::string errorMessage = "Usage: PRIVMSG <nickname> <message>\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
	}
	else
	{
		// Get the recipient's nickname first
		std::string recipientNick = params.front();
		std::cout << "recipient nick: " << recipientNick << std::endl;
		// params.pop_back();

		// Combine remaining params into the message
		std::string message;
		// while (!params.empty())
		// {
		// 	message = params.front() + (message.empty() ? "" : " " + message);
		// 	params.pop_back();
		// }
		for (size_t i = 1; i < params.size(); ++i) 
		{
			message += params[i] + ' ';
			std::cout << "message " << message << "\n";
			std::cout << "i  " << params[i] << "\n";
		}

		std::cout << "message: " << message << std::endl;

		std::string senderNick = _mapClients[clientFd].getNickName();
		std::string formattedMessage = senderNick + ": " + message + "\n";

		bool recipientFound = false;

		for (std::map<int, Client>::iterator it = _mapClients.begin(); it != _mapClients.end(); ++it)
		{
			// Access the Client object using it->second
			if (it->second.getNickName() == recipientNick)
			{
				// Client with the desired nickname found
				std::cout << "Client with nickname " << recipientNick << " found. Client FD: " << it->first << std::endl;
				send(it->first, formattedMessage.c_str(), formattedMessage.size(), 0);
				recipientFound = true;
				break;  // Exit loop once the client is found
			}
		}

		if (!recipientFound)
		{
			const std::string errorMessage = "No such user with nickname " + recipientNick + ".\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
}