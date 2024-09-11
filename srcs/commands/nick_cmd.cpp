#include "../../includes/Server.hpp"


void Server::nick_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
    if (params.size() != 0) // If a new nickname is provided
    {
        std::string newNick = params.front();
        std::string oldNick = client.getNickName();

        if (isNicknameInUse(newNick)) // Check if the new nickname is already in use
        {
            if (oldNick.empty())
            {
                sendMessage(clientFd, "Nickname is already in use.Reconnect using a different nick.\r\n"); 
                cleanupClient(clientFd);
                return;
            }
            else
            {
                sendMessage(clientFd, "Nickname is already in use.\r\n");
                return;
            }
        }

        if (!oldNick.empty()) // If the client already has a nickname
        {
            if (oldNick != newNick) // If the new nickname is different from the current one
            {
                // Set the new nickname
                client.setNickName(newNick);
                // Construct the NICK command response message
                std::string nickChangeMessage = ":" + oldNick + " NICK :" + newNick + "\r\n";
                // sendMessage the response back to the client
                sendMessage(clientFd, nickChangeMessage);
            }
            else // The new nickname is the same as the old one
                sendMessage(clientFd, "You are already using this nickname.\r\n");
        }
        else // The client does not have a nickname yet (first-time nickname assignment)
        {
            // Assign the new nickname
            client.setNickName(newNick);

            // Notify the client that their nickname has been set
            // Construct the NICK command response message
            std::string nickChangeMessage = ":" + client.getNickName() + " NICK :" + newNick + "\r\n";
            sendMessage(clientFd, nickChangeMessage);
        }
    }
    else // No new nickname is provided
    {
        sendMessage(clientFd, "Invalid nickname. Connection will be closed.\r\n");
        cleanupClient(clientFd);
    }
}



