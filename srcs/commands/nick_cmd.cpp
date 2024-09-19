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

        // Send the response back to the client
            send(clientFd, nickChangeMessage.c_str(), nickChangeMessage.size(), 0);

		if (client.getUserName().empty())
			sendMessage(clientFd, "Please set a username.\n");
		else if (client.getRegistratedWithPass() == false)
			sendMessage(clientFd, "Please set a password.\n");
		else
		{
			_authenticatedClients[clientFd] = true;
			const std::string welcomeUser = "Welcome, " + client.getNickName() + " (" + client.getUserName() + ")!\n";
			sendMessage(clientFd, welcomeUser);
			sendWelcomeMessageServe(clientFd);
		}
		return;

            // Optionally broadcast the nickname to other clients in the same channels
            // broadcastToChannels(nickChangeMessage, client);
        }
    }
    else // No new nickname is provided
    {
        sendMessage(clientFd, "Invalid nickname. Connection will be closed.\r\n");
        cleanupClient(clientFd);
    }
}




// #include "../../includes/Server.hpp"


// void Server::nick_cmd(Client &client, int clientFd, std::vector<std::string> params)
// {
//     if (params.size() != 0) // If a new nickname is provided
//     {
//         std::string newNick = params.front();
//         std::string oldNick = client.getNickName();

//         if (isNicknameInUse(newNick)) // Check if the new nickname is already in use
//         {
//             if (oldNick.empty())
//             {
//                 const std::string errorMessage = "Nickname is already in use.Reconnect using a different nick.\r\n";
//                 send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
//                 cleanupClient(clientFd);
//                 return;
//             }
//             else
//             {
//                 sendMessage(clientFd, "Nickname is already in use.\r\n");
//                 return;
//             }
//         }

//         if (!oldNick.empty()) // If the client already has a nickname
//         {
//             if (oldNick != newNick) // If the new nickname is different from the current one
//             {
//                 // Set the new nickname
//                 client.setNickName(newNick);
//                 // Construct the NICK command response message
//                 std::string nickChangeMessage = ":" + oldNick + " NICK :" + newNick + "\r\n";
//                 // sendMessage the response back to the client
//                 sendMessage(clientFd, nickChangeMessage);
//             }
//             else // The new nickname is the same as the old one
//                 sendMessage(clientFd, "You are already using this nickname.\r\n");
//         }
//         else // The client does not have a nickname yet (first-time nickname assignment)
//         {
//             // Assign the new nickname
//             client.setNickName(newNick);

//             // Notify the client that their nickname has been set
//             // Construct the NICK command response message
//             std::string nickChangeMessage = ":" + client.getNickName() + " NICK :" + newNick + "\r\n";

//         // Send the response back to the client
//             send(clientFd, nickChangeMessage.c_str(), nickChangeMessage.size(), 0);

//             // Optionally broadcast the nickname to other clients in the same channels
//             // broadcastToChannels(nickChangeMessage, client);
//         }
//     }
//     else // No new nickname is provided
//     {
//         sendMessage(clientFd, "Invalid nickname. Connection will be closed.\r\n");
//         cleanupClient(clientFd);
//     }
// }




