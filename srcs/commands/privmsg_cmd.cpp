#include "../../includes/Server.hpp"

void Server::privmsg_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
    if (params.size() < 2)
    {
        sendMessage(clientFd,"Usage: PRIVMSG <nickname|#channel> <message>\n");
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

    std::string sendMessagerNick = _mapClients[clientFd].getNickName();
    std::string formattedMessage = ":" + sendMessagerNick + " PRIVMSG " + recipient + " :" + message + "\r\n";

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
                sendMessage(it->first, formattedMessage);
                recipientFound = true;
                break;  // Exit loop once the client is found
            }
        }
        if (!recipientFound)
        {
            std::string errorMessage = ":401 " + sendMessagerNick + " " + recipient + " :No such nick/channel\n";
            sendMessage(clientFd, "No such user with nickname " + recipient + ".\n");
        }
    }
}

