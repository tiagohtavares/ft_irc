#include "../../includes/Server.hpp"

void	Server::pass_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
    if (params.size() == 1)
    {
        std::cout << "Received password: " << params.front() << std::endl;
        std::cout << "Params size: " << params.size() << std::endl;

        if (params.front() == _password)
        {
			client.setRegisteredWithPass(true);

			if (client.getNickName().empty())
				sendMessage(clientFd, "Please set a nickname.\n");
			else if (client.getUserName().empty())
				sendMessage(clientFd, "Please set a username.\n");
			else
			{
				_authenticatedClients[clientFd] = true;
				const std::string welcomeUser = "Welcome, " + client.getNickName() + " (" + client.getUserName() + ")!\n";
				sendMessage(clientFd, welcomeUser);
				sendWelcomeMessageServe(clientFd);
			}
			return;
        }
        else
        {
            sendMessage(clientFd, "Invalid password. Connection will be closed.\n");
            cleanupClient(clientFd);
        }
    }
    else
    {
        sendMessage(clientFd, "Invalid command. Connection will be closed.\n");
        cleanupClient(clientFd);
    }
}
