#include "../../includes/Server.hpp"

void	Server::user_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
				if (params.size() >= 4) // Typically, USER command has 4 parameters
				{
					std::string username = params.front(); params.pop_back();
					std::string mode = params.front(); params.pop_back(); // Mode is typically ignored but pop_backped
					std::string unused = params.front(); params.pop_back(); // Unused parameter, typically '*'
					std::string realName = params.front(); params.pop_back(); // Real name

					if (client.getUserName().empty()) // Set the username if not already set
					{
						client.setUserName(username);
						client.setRealName(realName);

						// sendMessage a welcome message after USER is set
						const std::string welcomeUser = "Welcome, " + client.getNickName() + " (" + client.getUserName() + ")!\n";
						sendMessage(clientFd,welcomeUser);
					
					}
					else
						sendMessage(clientFd, "Username already set.\n");
				}
				else
				{
					sendMessage(clientFd, "Invalid USER command. Connection will be closed.\n");
					cleanupClient(clientFd);
					return;
				}
}