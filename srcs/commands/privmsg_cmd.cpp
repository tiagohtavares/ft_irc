#include "../../includes/Server.hpp"

void Server::privmsg_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
    if (params.size() < 2)
    {
        const std::string errorMessage = "Usage: PRIVMSG <nickname|#channel> <message>\n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
    }

    // Get the recipient's nickname or channel name
    std::string recipient = params.front();
    std::cout << "Recipient: " << recipient << std::endl;

    // Combine remaining params into the message
    std::string message;
    for (size_t i = 1; i < params.size(); ++i)
    {
        message += params[i] + ' ';
    }
    std::cout << "Message: " << message << std::endl;

    std::string senderNick = _mapClients[clientFd].getNickName();
    std::string formattedMessage = ":" + senderNick + " PRIVMSG " + recipient + " :" + message + "\r\n";

    if (recipient[0] == '#')  // Check if the recipient is a channel
    {
       msg_cmd(client, clientFd, params);
    }
    else // Assume the recipient is a user
    {
        bool recipientFound = false;

        for (std::map<int, Client>::iterator it = _mapClients.begin(); it != _mapClients.end(); ++it)
        {
            if (it->second.getNickName() == recipient)
            {
                // Client with the desired nickname found
                std::cout << "Client with nickname " << recipient << " found. Client FD: " << it->first << std::endl;
                send(it->first, formattedMessage.c_str(), formattedMessage.size(), 0);
                recipientFound = true;
                break;  // Exit loop once the client is found
            }
        }

        if (!recipientFound)
        {
            const std::string errorMessage = "No such user with nickname " + recipient + ".\n";
            send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        }
    }
}

