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
	//- Verifique se cada chamada para fcntl() é feita da seguinte maneira:
	// fcntl(fd, F_SETFL, O_NONBLOCK); Qualquer outro uso de fcntl() é proibido.
	//Segundo a avalição.
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
		_params.pop_back();

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


void Server::printParams() const
{
    std::cout << "Command: " << _cmd << "\n";
    std::cout << "Parameters:\n";
    for (size_t i = 0; i < _params.size(); ++i) {
        std::cout << "Param " << i + 1 << ": " << _params[i] << "\n";
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

	//-----PRINT cmd and params-----//
	std::cout << "cmd: " << cmd << std::endl;

	for (std::size_t i = 0; i < params.size(); i++){std::cout << "params: " << params[i] << std::endl;}

	//------------------TO DO ----------------------
	//Generate proper responses to client after recieving command
	if (!cmd.empty())
	{
		if(!_authenticatedClients[clientFd])
		{
			if (cmd == "CAP"){return;}
			else
				pass_cmd(clientFd, params);
		}
		if(_authenticatedClients[clientFd])
		{
			if (cmd == "NICK")
				nick_cmd(client, clientFd, params);
			else if (cmd == "USER" && client.getUserName().empty())
				user_cmd(client, clientFd, params);
			else if (cmd == "PRIVMSG")
				privmsg_cmd(clientFd, params);
			else if (cmd == "MSG")	// This command is pointless i think.
				msg_cmd(client, clientFd, params);
			else if (cmd == "JOIN")
				join_cmd(client, clientFd, params);
			else if (cmd == "TOPIC" && params.size() >= 1)
				topic_cmd(client, clientFd, params);
			else if (cmd == "PART")
				part_cmd(client, clientFd, params);
			else if (cmd == "KICK")
				kick_cmd(client, clientFd, params);
			else if (cmd == "MODE") // /mode <your_nickname> +/-<mode>
				mode_cmd(client, clientFd, params);
			else if (cmd == "LIST")
				listChannels(client);
			else if (cmd == "NAMES")
				names_cmd(clientFd, params);
			else if(cmd == "QUIT")
				quit_cmd(clientFd);
			else if (cmd.empty() && params.empty())
				return;
			else
			{
				if(cmd != "PASS")
				{
					send(clientFd, cmd.c_str(), cmd.size(), 0);
					const std::string Message = ": invalid command\n";
					send(clientFd, Message.c_str(), Message.size(), 0);
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

void	Server::listChannels(Client &client)
{
	std::string channelList;
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		channelList += it->first + "\n";
	}
	send(client.getClientFd(), channelList.c_str(), channelList.size(), 0);
}
