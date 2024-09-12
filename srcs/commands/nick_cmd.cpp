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
                const std::string errorMessage = "Nickname is already in use.Reconnect using a different nick.\r\n";
                send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
                cleanupClient(clientFd);
                return;
            }
            else
            {
                const std::string errorMessage = "Nickname is already in use.\r\n";
                send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
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

                // Send the response back to the client
                send(clientFd, nickChangeMessage.c_str(), nickChangeMessage.size(), 0);

                // Optionally broadcast the nickname change to other clients in the same channels
                // broadcastToChannels(nickChangeMessage, client);
            }
            else // The new nickname is the same as the old one
            {
                // Optionally handle the case where the nickname is the same
                const std::string message = "You are already using this nickname.\r\n";
                send(clientFd, message.c_str(), message.size(), 0);
            }
        }
        else // The client does not have a nickname yet (first-time nickname assignment)
        {
            // Assign the new nickname
            client.setNickName(newNick);

            // Notify the client that their nickname has been set
            // Construct the NICK command response message
            std::string nickChangeMessage = ":" + client.getNickName() + " NICK :" + newNick + "\r\n";

        // Send the response back to the client
            send(clientFd, nickChangeMessage.c_str(), nickChangeMessage.size(), 0);

            // Optionally broadcast the nickname to other clients in the same channels
            // broadcastToChannels(nickChangeMessage, client);
        }
    }
    else // No new nickname is provided
    {
        // Error message for invalid nickname command
        const std::string errorMessage = "Invalid nickname. Connection will be closed.\r\n";

        // Send the error message back to the client
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);

        // Cleanup the client (assuming this function disconnects the client)
        cleanupClient(clientFd);
    }
}



