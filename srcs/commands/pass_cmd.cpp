#include "../../includes/Server.hpp"

void	Server::pass_cmd(int clientFd, std::vector<std::string> params)
{
    if (params.size() == 1)
    {
        std::cout << "Received password: " << params.front() << std::endl;
        std::cout << "Params size: " << params.size() << std::endl;
        
        if (params.front() == _password)
        {
            _authenticatedClients[clientFd] = true;
            const std::string welcomeMessage = "Password accepted. You are now connected.\n";
            if (send(clientFd, welcomeMessage.c_str(), welcomeMessage.size(), 0) == -1)
            {
                std::cerr << "Failed to send welcome message to client." << std::endl;
            }
            std::cout << "Client authenticated." << std::endl;
        }
        else
        {
            const std::string errorMessage = "Invalid password. Connection will be closed.\n";
            if (send(clientFd, errorMessage.c_str(), errorMessage.size(), 0) == -1)
            {
                std::cerr << "Failed to send error message to client." << std::endl;
            }
            cleanupClient(clientFd);
        }
    }
    else
    {
        const std::string errorMessage = "Invalid command. Connection will be closed.\n";
        if (send(clientFd, errorMessage.c_str(), errorMessage.size(), 0) == -1)
        {
            std::cerr << "Failed to send error message to client." << std::endl;
        }
        cleanupClient(clientFd);
    }
}