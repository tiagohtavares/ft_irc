#include "../includes/Server.hpp"

Server::Server(int port, const std::string &password) : _port(port), _password(password), _server_fd(-1), _mapClients(), _cmd(), _params()
{
	Channel defaultChannel;
}

Server::~Server()
{
	for (size_t i = 0; i < _pollfds.size(); ++i)
	{
		close(_pollfds[i].fd);
	}
	_pollfds.clear();
}


void Server::start()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1)
	{
		std::cerr << "Failed to create socket! Error: "<< std::endl;
		exit(0);
	}

	int flags = fcntl(_server_fd, F_GETFL, 0);
	if (flags == -1 || fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Failed to set non-blocking mode! Error: "<< std::endl;
		close(_server_fd);
		exit(0);
	}

	// Specify socket address
	_serverAddress.sin_family = AF_INET; // IPv4
	_serverAddress.sin_port = htons(_port); // Convertida p/ a ordem de bytes de rede
	_serverAddress.sin_addr.s_addr = INADDR_ANY; // Qualquer endereço IP disponível

	if (bind(_server_fd, (sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1)
	{
		std::cerr << "Binding failed! Error: "<< std::endl;
		close(_server_fd);
		exit(0);
	}

	if (listen(_server_fd, 5) == -1)
	{
		std::cerr << "Listening failed! Error: "<< std::endl;
		close(_server_fd);
		exit(0);
	}

	std::cout << "Server is running and waiting for connections..." << std::endl << std::flush;// std::flush evita que o buffer seja limpo antes de imprimir a mensagem
}

void Server::run()
{
    start();

    struct pollfd serverPollFd;
    serverPollFd.fd = _server_fd;
    serverPollFd.events = POLLIN; // Monitor for incoming connections
    serverPollFd.revents = 0;
    _pollfds.push_back(serverPollFd);

    bool running = true;

    while (running)
    {
        int pollResult = poll(_pollfds.data(), _pollfds.size(), -1);
        if (pollResult == -1)
        {
            std::cerr << "Poll failed! Error: "<< std::endl;
            break;
        }

        // Handle new connection
        if (_pollfds[0].revents & POLLIN)
        {
            handleNewConnection();
        }

        // Handle client activity
        for (size_t i = 1; i < _pollfds.size(); ++i)
        {
            if (_pollfds[i].revents & POLLIN)
            {
                char buffer[4096];
                memset(buffer, 0, sizeof(buffer));
                int bytesReceived = recv(_pollfds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesReceived == -1)
                {
                    std::cerr << "Failed to receive message from client. Error: " << std::endl;
                    cleanup();
                }
                else if (bytesReceived == 0)
                {
                    std::cout << "Client disconnected" << std::endl;
                    cleanup();
                }
                else
                {
                    std::string receivedMessage(buffer, bytesReceived);
					std::cout << "Received message: " << receivedMessage << std::endl;
                    size_t pos = 0;
                    while ((pos = receivedMessage.find("\r\n")) != std::string::npos)
                    {
                        std::string singleMessage = receivedMessage.substr(0, pos);
                        receivedMessage.erase(0, pos + 2); // Remove the processed message
                        splitCmdLine(singleMessage);
                        processClientMessage(_pollfds[i].fd, _cmd, _params);
                    }
                }
            }
        }
    }
}


void Server::handleNewConnection()
{
	Client		client;
	sockaddr_in	clientAddress;// evdos-sa: incorporar na classe Client

	socklen_t clientSize = sizeof(clientAddress);

	// client.setClientFd(accept(_server_fd, (sockaddr*)&clientAddress, &clientSize));
	int clientFd = accept(_server_fd, (sockaddr*)&clientAddress, &clientSize);
	if (clientFd == -1)
	{
		std::cerr << "Failed to accept connection! Error: " << std::endl;
		return;
	}

	// Set the new socket to non-blocking mode
	int flags = fcntl(clientFd, F_GETFL, 0);
	if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Failed to set non-blocking mode for client socket! Error: " << std::endl;
		close(clientFd);
		return;
	}

	// Add new client socket to pollfds
	struct pollfd clientPollFd;
	clientPollFd.fd = clientFd;
	clientPollFd.events = POLLIN; // Monitor for incoming data
	clientPollFd.revents = 0;
	_pollfds.push_back(clientPollFd);

	// Store the new client
	client.setClientFd(clientFd);
	_mapClients[clientFd] = client;
	_authenticatedClients[clientFd] = false;

	std::cout << "Client connected from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;
}

void Server::splitCmdLine(std::string input)
{
	while (!_params.empty())
		_params.pop();

	input = input.substr(input.find_first_not_of(" "));
	size_t space_pos = input.find_first_of(" ");
	_cmd = input.substr(0, space_pos);

	size_t i;
	i = 0;

	while (i < _cmd.size())
	{
		if (std::islower (_cmd[i]))
			_cmd[i] = std::toupper (_cmd[i]);
		i++;
	}

	if (space_pos != std::string::npos)
		++space_pos;
	input.erase(0, space_pos);

	while (!input.empty())
	{
		input = input.substr(input.find_first_not_of(" "));
		if (input[0] == ':')
		{
			input.erase(input.begin());
			if (!input.empty())
				_params.push(input);
			break;
		}
		space_pos = input.find_first_of(" ");
		if (space_pos == std::string::npos)
		{
			_params.push(input);
			break;
		}
		else
		{
			_params.push(input.substr(0, space_pos));
			input.erase(0, ++space_pos);
		}
	}
}




void Server::processClientMessage(int clientFd, std::string cmd, std::stack<std::string> params)
{
	Client &client = _mapClients[clientFd];

	std::cout << "cmd: " << cmd << std::endl;

		std::vector<std::string> temp;

	std::queue<std::string> queue;

	// Transfer elements from stack to queue
	while (!params.empty())
	{
		queue.push(params.top());
		params.pop();
	}

	// Transfer elements from queue back to stack
	while (!queue.empty())
	{
		params.push(queue.front());
		queue.pop();
	}

	// Traverse the stack
	while (!params.empty())
	{
		// Pop the top element
		std::string topElement = params.top();
		params.pop();

		// Print the element
		std::cout << "params: " << topElement << std::endl;

		// Store it in the temporary container
		temp.push_back(topElement);
	}

	std::vector<std::string>::reverse_iterator it;
	for (it = temp.rbegin(); it != temp.rend(); ++it)
	{
		params.push(*it);
	}

	//------------------TO DO ----------------------
	//Generate proper responses to client after recieving command
	if (!cmd.empty())
	{
		if(!_authenticatedClients[clientFd])
		{
			if (cmd == "CAP")
			{
				return;
			}
			else if (cmd == "PASS")
			{
				std::cout << "Received password: " << params.top() << std::endl;
				std::cout << "Params size: " << params.size() << std::endl;
				if (params.size() == 1)
				{
					if (params.top() == _password)
					{
						_authenticatedClients[clientFd] = true;
						const std::string welcomeMessage = "Password accepted. You are now connected.\n";
						send(clientFd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
						std::cout << "Client authenticated." << std::endl;
						// params.pop();
					}
					else
					{
						const std::string errorMessage = "Invalid password. Connection will be closed.\n";
						send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
						cleanupClient(clientFd);
                        return;
					}
				}
			}
			else
			{
				const std::string errorMessage = "Invalid command. Connection will be closed.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
				cleanupClient(clientFd);
				return;
			}
		}
		if(_authenticatedClients[clientFd])
		{
			if (cmd == "NICK" )
			{
				if (params.size() != 0) 		//Lidar com situacao de 2 nicks iguais ------------!!!!!!!!!!!!!!!!!!
				{
					client.setNickName(params.top());
				}
				else
				{
					const std::string errorMessage = "Invalid nickname. Connection will be closed.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					cleanupClient(clientFd);
					return;
				}
			}
			else if (cmd == "USER" && client.getUserName().empty())
			{
				if (params.size() >= 4) // Typically, USER command has 4 parameters
				{
					std::string username = params.top(); params.pop();
					std::string mode = params.top(); params.pop(); // Mode is typically ignored but popped
					std::string unused = params.top(); params.pop(); // Unused parameter, typically '*'
					std::string realName = params.top(); params.pop(); // Real name

					if (client.getUserName().empty()) // Set the username if not already set
					{
						client.setUserName(username);
						client.setRealName(realName);

						// Send a welcome message after USER is set
						const std::string welcomeUser = "Welcome, " + client.getNickName() + " (" + client.getUserName() + ")!\n";
						send(clientFd, welcomeUser.c_str(), welcomeUser.size(), 0);
					}
					else
					{
						const std::string errorMessage = "Username already set.\n";
						send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					}
				}
				else
				{
					const std::string errorMessage = "Invalid USER command. Connection will be closed.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					cleanupClient(clientFd);
					return;
				}
			}
			else if (cmd == "PRIVMSG")
			{
				if (params.size() < 2)
				{
					const std::string errorMessage = "Usage: PRIVMSG <nickname> <message>\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
				}
				else
				{
					// Get the recipient's nickname first
					std::string recipientNick = params.top();
					std::cout << "recipient nick: " << recipientNick << std::endl;
					params.pop();

					// Combine remaining params into the message
					std::string message;
					while (!params.empty())
					{
						message = params.top() + (message.empty() ? "" : " " + message);
						params.pop();
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
			else if (cmd == "JOIN")
			{
				if (params.size() == 1 && params.top()[0] == '#' && params.top().size() == 1)
				{
						const std::string message = "/JOIN <#channel_name>\n";
						send(clientFd, message.c_str(), message.size(), 0);
				}
				else if (params.size() == 1 && params.top().size() > 1 && params.top()[0] == '#' && params.top()[1] == '#')
				{
						const std::string message = "The channel name cannot start with #.\n";
						send(clientFd, message.c_str(), message.size(), 0);
				}
				else if (params.size() == 1 && params.top()[0] == '#' && params.top().size() > 1)
				{
					/*
						* Everton:
						- Check if the channel exists
							- Check if the client is already in the channel
								- If the client is already in the channel, send a message
								- If the client is not in the channel, add it
						- If the channel does not exist, create it and add the client
					*/
					if (isChannelExist(params.top()))
					{
						if (isClientInChannel(params.top(), client))
						{
							// Ajustar a mensagem de erro para apenas quando o cliente ja estiver no canal.
							// const std::string errorMessage = "You are already in the " + params.top() + " channel.\n";
							const std::string channelTopic = _channels[params.top()].getTopic() + "\n";
							send(clientFd, channelTopic.c_str(), channelTopic.size(), 0);
						}
						else
						{
							_channels[params.top()].insertMember(client);
						}
					}
					else
					{
						std::string channelName = params.top().substr(1);
						createChannel(channelName, client);
						// createChannel(params.top(), client);
					}
					params.pop();

					// Last implementation
					// std::string channelName = params.top();
					// params.pop();
					// createChannel(channelName, client);
				}
				else
				{
					const std::string errorMessage = "Invalid JOIN command. Connection will be closed.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					// cleanupClient(clientFd);
					// return;
				}
			}
			else if (cmd == "PART")
			{
				if (params.size() == 1)
				{
					if (isChannelExist(params.top()))
					{
						if (isClientInChannel(params.top(), client))
						{
							_channels[params.top()].removeMember(client);
						}
						else
						{
							const std::string errorMessage = "You are not in the " + params.top() + " channel.\n";
							send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
						}
					}
					else
					{
						const std::string errorMessage = "The " + params.top() + " channel does not exist.\n";
						send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					}
					params.pop();
				}
				else
				{
					const std::string errorMessage = "Invalid PART command. Connection will be closed.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					cleanupClient(clientFd);
					return;
				}
			}
			else if (cmd == "LIST")
			{
				listChannels(client);
			}
			else if (cmd.empty() && params.empty())
			{
				return ;
			}
			else
			{
				// std::cout << "Message from " << client.getNickName() << ": " << receivedMessage << std::endl;
				if (cmd == "QUIT")
				{
					std::cout << "Received close message. Shutting down client." << std::endl;
					cleanupClient(clientFd);
				}
			}
		}
	}
}

void Server::cleanupClient(int clientFd) {
	std::cout << "Cleaning up client " << clientFd << std::endl;
	close(clientFd);
	_authenticatedClients.erase(clientFd);
	_mapClients.erase(clientFd);

	for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		if (it->fd == clientFd) {
			_pollfds.erase(it);
			break;
		}
	}
}

void Server::cleanup()
{
	for (size_t i = 1; i < _pollfds.size(); ++i)
	{
		close(_pollfds[i].fd);
		_pollfds.erase(_pollfds.begin() + i);
		--i;
	}
}


/*
	* To do: check if the channel is private before inserting the client;
	* DONE: After the customer has already entered the channel, he cannot enter again;
*/
void Server::createChannel(std::string &channelName, Client &client)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
	{
		_channels.insert(std::make_pair(channelName, Channel(channelName, client)));
	}
	else
	{
		it->second.insertMember(client);
	}
}

bool	Server::isChannelExist(std::string &channelName)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
	{
		return (false);
	}
	return (true);
}

bool	Server::isClientInChannel(std::string &channelName, Client &client)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
	{
		if (it->second.getMembers().find(client.getClientFd()) != it->second.getMembers().end())
		{
			return (true);
		}
	}
	return (false);
}

void	Server::deleteChannel(std::string &channelName)
{
	if (_channels.find(channelName) != _channels.end())
	{
		_channels.erase(channelName);
	}
}

void	Server::listChannels(Client &client)
{
	std::string channelList;
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		channelList += it->first + "\n";
	}
	send(client.getClientFd(), channelList.c_str(), channelList.size(), 0);
}

