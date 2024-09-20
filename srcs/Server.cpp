#include "../includes/Server.hpp"

volatile bool g_running = true;

void signalHandler(int signum)
{
    if (signum == SIGINT)
	{
        std::cout << "\nSIGINT received. Shutting down gracefully...\n";
        g_running = false;  // Set the running flag to false
    }
}

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
		std::cerr << "Failed to create socket! Error: " << std::endl;
		exit(1);
	}

	// Set the server socket to non-blocking mode directly
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Failed to set non-blocking mode! Error: " << std::endl;
		close(_server_fd);
		exit(1);
	}

	// Specify socket address
	_serverAddress.sin_family = AF_INET;            // IPv4
	_serverAddress.sin_port = htons(_port);         // Convert to network byte order
	_serverAddress.sin_addr.s_addr = INADDR_ANY;    // Any available IP address

	if (bind(_server_fd, (sockaddr*)&_serverAddress, sizeof(_serverAddress)) == -1)
	{
		std::cerr << "Binding failed! Error: " << std::endl;
		close(_server_fd);
		exit(1);
	}

	if (listen(_server_fd, 5) == -1)
	{
		std::cerr << "Listening failed! Error: " << std::endl;
		close(_server_fd);
		exit(1);
	}

	std::cout << "Server is running and waiting for connections..." << std::endl << std::flush;
}


void Server::run()
{
	start();
	signal(SIGINT, signalHandler);
	struct pollfd serverPollFd;
	serverPollFd.fd = _server_fd;
	serverPollFd.events = POLLIN; // Monitor for incoming connections
	serverPollFd.revents = 0;
	_pollfds.push_back(serverPollFd);

	while (g_running)
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
					cleanupClient(_pollfds[i].fd);
				}
				else if (bytesReceived == 0)
				{
					std::cout << "Client disconnected" << std::endl;
					cleanupClient(_pollfds[i].fd);
				}
				else
				{
					// Add received data to the client's buffer
					_clientBuffers[_pollfds[i].fd] += std::string(buffer, bytesReceived);

					// Process complete commands
					processClientData(_pollfds[i].fd);
				}
			}
		}
	}
}

std::string sanitizeInput(const std::string& input)
{
    std::string result;
    for (size_t i = 0; i < input.size(); ++i) {
        if (isprint(input[i]) || input[i] == '\r' || input[i] == '\n') {
            result += input[i];
        }
    }
    return result;
}

void Server::processClientData(int clientFd)
{
    std::string &buffer = _clientBuffers[clientFd];

    // Sanitize input to remove unwanted control characters
    buffer = sanitizeInput(buffer);

    size_t pos;
	while ((pos = buffer.find_first_of("\r\n")) != std::string::npos)
	{
		std::string singleMessage = buffer.substr(0, pos);
		buffer.erase(0, pos + 1); // Remove o '\r' ou '\n' que foi encontrado

		// Se a próxima posição for o outro caractere (ou seja, '\r\n' encontrado), remove também
		if (!buffer.empty() && (buffer[0] == '\r' || buffer[0] == '\n')) {
			buffer.erase(0, 1); // Remove o segundo caractere ('\r' ou '\n')
		}
		splitCmdLine(singleMessage);
		processClientMessage(clientFd, _cmd, _params);
	}

}


void Server::handleNewConnection()
{
	Client		client;
	sockaddr_in	clientAddress;

	socklen_t clientSize = sizeof(clientAddress);

	int clientFd = accept(_server_fd, (sockaddr*)&clientAddress, &clientSize);
	if (clientFd == -1)
	{
		std::cerr << "Failed to accept connection! Error: " << std::endl;
		return;
	}

	// Set the new socket to non-blocking mode directly
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
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
	client.setClientAddress(inet_ntoa(clientAddress.sin_addr));
	_mapClients[clientFd] = client;
	_authenticatedClients[clientFd] = false;

	std::cout << "Client connected from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;
	// sendWelcomeMessageServe(clientFd);
}

void Server::splitCmdLine(std::string input)
{
	while (!_params.empty())
		_params.pop_back();

	if (input.empty())
		return;

	input = input.substr(input.find_first_not_of(" "));
	size_t space_pos = input.find_first_of(" ");
	_cmd = input.substr(0, space_pos);

	for (size_t i = 0; i < _cmd.size(); i++)
	{
		if (std::islower(_cmd[i]))
			_cmd[i] = std::toupper(_cmd[i]);
	}

	if (space_pos != std::string::npos)
		++space_pos;
	input.erase(0, space_pos);

	while (!input.empty())
	{
		// Remove espaços em branco no início da string
		size_t first_not_space = input.find_first_not_of(" ");
		if (first_not_space == std::string::npos)
			break; // Não há mais parâmetros

		input = input.substr(first_not_space);
		if (input[0] == ':')
		{
			input.erase(input.begin());
			if (!input.empty())
				_params.push_back(input);
			break;
		}
		space_pos = input.find_first_of(" ");
		if (space_pos == std::string::npos)
		{
			_params.push_back(input);
			break;
		}
		else
		{
			_params.push_back(input.substr(0, space_pos));
			input.erase(0, ++space_pos);
		}
	}
}

bool Server::isNicknameInUse(const std::string &nick) const
{
    // Explicit iterator type for the map
    std::map<int, Client>::const_iterator it;

    for (it = _mapClients.begin(); it != _mapClients.end(); ++it)
	{
        if (it->second.getNickName() == nick)
		{
            return true;
        }
    }
    return false;
}

void Server::processClientMessage(int clientFd, std::string cmd, std::vector<std::string> params)
{
	Client &client = _mapClients[clientFd];

	std::cout << "cmd: " << cmd << std::endl;
	for (std::size_t i = 0; i < params.size(); i++){std::cout << "params: " << params[i] << std::endl;}//-----PRINT cmd and params-----//

	if (!cmd.empty())
	{
		if(!_authenticatedClients[clientFd])
		{
			if (cmd == "CAP"){return;}
			else if (cmd == "PASS"){pass_cmd(client, clientFd, params);return;}
			else if (cmd == "NICK"){nick_cmd(client, clientFd, params);return;}
			else if (cmd == "USER"){user_cmd(client, clientFd, params);return;}
		}
		if(_authenticatedClients[clientFd])
		{

			if (cmd == "NICK")
				nick_cmd(client, clientFd, params);
			else if (cmd == "USER" && client.getUserName().empty())
				user_cmd(client, clientFd, params);
			else if (cmd == "WHOIS")
				whois_cmd(client, params);
			else if (cmd == "PRIVMSG")
				privmsg_cmd(client,clientFd, params);
			else if (cmd == "INVITE")
				invite_cmd(client, clientFd, params);
			else if (cmd == "MSG")
				msg_cmd(client, clientFd, params);
			else if (cmd == "JOIN")
				join_cmd(client, clientFd, params);
			else if (cmd == "TOPIC" && params.size() >= 1)
				topic_cmd(client, clientFd, params);
			else if (cmd == "PART")
				part_cmd(client, clientFd, params);
			else if (cmd == "KICK")
				kick_cmd(client, clientFd, params);
			else if (cmd == "MODE" && params.size() >= 2)
				mode_cmd(client, clientFd, params);
			else if (cmd == "LIST")
				listChannels(client);
			else if (cmd == "NAMES")
				names_cmd(client, clientFd, params);
			else if(cmd == "QUIT")
				quit_cmd(client);
			else if (cmd.empty() && params.empty())
				return;
			else
			{
				if(cmd != "PASS")
				{
					sendMessage(clientFd, cmd);
					sendMessage(clientFd, ": invalid command\n");
				}
			}
		}
	}
}


// void Server::cleanupClient(int clientFd) {
// 	std::cout << "Cleaning up client " << clientFd << std::endl;
// 	close(clientFd);
// 	_authenticatedClients.erase(clientFd);
// 	_mapClients.erase(clientFd);

// 	for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it)
// 	{
// 		if (it->fd == clientFd)
// 		{
// 			_pollfds.erase(it);
// 			break;
// 		}
// 	}
// }


void Server::cleanupClient(int clientFd) {
    std::cout << "Cleaning up client " << clientFd << std::endl;

    // Primeiro, remova o cliente de todos os canais
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        Channel &channel = it->second;

        if (channel.isCreator(_mapClients[clientFd]))
        {
            channel.removeMember(_mapClients[clientFd]);
            channel.removeCreator(_mapClients[clientFd]);
            channel.removeOperator(_mapClients[clientFd]);
            channel.removeInvited(_mapClients[clientFd]);
        }
        else if (channel.isOperator(_mapClients[clientFd]))
        {
            channel.removeOperator(_mapClients[clientFd]);
            channel.removeMember(_mapClients[clientFd]);
            channel.removeInvited(_mapClients[clientFd]);
        }
        else
        {
            channel.removeMember(_mapClients[clientFd]);
            channel.removeInvited(_mapClients[clientFd]);
        }
    }

    // Feche a conexão do cliente
    close(clientFd);

    // Remova o cliente das listas internas
	if (_authenticatedClients.find(clientFd) != _authenticatedClients.end())
	{
		_authenticatedClients.erase(clientFd);
	}
	_authenticatedClients.erase(clientFd);
    // Remova o cliente das listas internas
	if (_mapClients.find(clientFd) != _mapClients.end())
	{
    	_mapClients.erase(clientFd);
	}
    // _authenticatedClients.erase(clientFd);
    // _mapClients.erase(clientFd);

    // Remova o fd do cliente da lista de pollfds
    for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
        if (it->fd == clientFd) {
            _pollfds.erase(it);
            break;
        }
    }
}


// void Server::cleanupClient(int clientFd) {
//     std::cout << "Cleaning up client " << clientFd << std::endl;

//     leaveAllChannels(_mapClients[clientFd]);

//     // Feche a conexão do cliente
//     close(clientFd);

//     // Remova o cliente das listas internas
//     _authenticatedClients.erase(clientFd);
//     _mapClients.erase(clientFd);

//     // Remova o fd do cliente da lista de pollfds
//     for (std::vector<struct pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
//         if (it->fd == clientFd) {
//             _pollfds.erase(it);
//             break;
//         }
//     }
// }

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



bool Server::isChannelExist(const std::string &channelName) const
{
	std::map<std::string, Channel>::const_iterator it = _channels.find(channelName);
	if (it == _channels.end())
	{
		return false;
	}
	return true;
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


Channel* Server::getChannelByName(const std::string& name)
{
    std::map<std::string, Channel>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        return &(it->second);  // Return a pointer to the channel
    }
    return NULL;  // Channel not found
}

void	Server::leaveAllChannels(Client &client)
{
	std::map<std::string, Channel>::iterator itChannel = _channels.begin();
	while (itChannel != _channels.end())
	{
		if (!itChannel->second.getMembers().empty() && itChannel->second.isMember(client))
		{
			std::string channelName = itChannel->second.getChannelName();
			std::string nickname = client.getNickName();
			std::string leaveMessage = ":" + client.getNickName() + " PART " + itChannel->second.getChannelName() + "\r\n";

			if (itChannel->second.isCreator(client))
			{
				itChannel->second.removeCreator(client);
				itChannel->second.removeOperator(client);
				itChannel->second.removeMember(client);
				itChannel->second.removeInvited(client);
			}
			else if (itChannel->second.isOperator(client))
			{
				itChannel->second.removeOperator(client);
				itChannel->second.removeMember(client);
				itChannel->second.removeInvited(client);
			}
			else
			{
				itChannel->second.removeMember(client);
				itChannel->second.removeInvited(client);
			}
			if (_channels[channelName].getMembers().size() == 0)
			{
				++itChannel;
				deleteChannel(channelName);
				continue;
			}
			else
			{
				sendToChannel(itChannel->second.getChannelName(), leaveMessage);
			}
			if (isChannelExist(channelName) && !_channels[channelName].isMember(client))
			{
				std::string errorMessage = ":442 " + nickname + " " + channelName + " :You're not on that channel\n";
				send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
				++itChannel;
				continue;
			}

		}
		if (_channels.empty())
		{
			break;
		}
		++itChannel;
	}
}
