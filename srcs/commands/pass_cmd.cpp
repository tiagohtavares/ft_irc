#include "../../includes/Server.hpp"

void	Server::pass_cmd(int clientFd, std::vector<std::string> params)
{
    if (params.size() == 1)
    {
        std::cout << "Received password: " << params.front() << std::endl;
        std::cout << "Params size: " << params.size() << std::endl;

        if (params.front() == _password)
        {
            // _authenticatedClients[clientFd] = true;
            sendMessage(clientFd, "Welcome to the server.\nTo start a session, enter a nickname and username.\n");
            std::cout << "Client authenticated." << std::endl;
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
