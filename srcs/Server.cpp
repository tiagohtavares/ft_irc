#include "../includes/Server.hpp"

Server::Server(int port, const std::string &password) : _port(port), _password(password), _server_fd(-1), _mapClients(), _cmd(), _params()
{
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
				// std::fill(buffer, buffer + sizeof(buffer), 0);// evdos-sa: substitui memset por std::fill
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
					receivedMessage.erase(receivedMessage.find_last_not_of("\r\n") + 1);
					std::cout << "Received message " << receivedMessage << std::endl;
					processClientMessage(_pollfds[i].fd, receivedMessage);
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

	// // Send a password prompt to the client
	// const std::string passwordPrompt = "Please enter the password:\n";
	// send(client.getClientFd(), passwordPrompt.c_str(), passwordPrompt.size(), 0);

	std::cout << "Client connected from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;
}

void Server::splitCmdLine(std::string input)
{
	// enquanto a stack nao estiver vazia, limpa
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

void Server::processClientMessage(int clientFd, const std::string &receivedMessage)
{
	Client &client = _mapClients[clientFd];

	splitCmdLine(receivedMessage);
	// if (!_cmd.empty())
	// 	std::cout << "Command: " << _cmd << std::endl;
	// if (!_params.empty())
	// 	std::cout << "Params:  " << _params.top() << std::endl;

	if (!_cmd.empty())
	{
		if(!_authenticatedClients[clientFd])
		{
			// Temos de tratar a situacao de CAP mais a frente
			// std::cout << "Client " << clientFd <<  "not auth" << std::endl;
			if (_cmd == "CAP")
			{
				return;
			}
			else if (_cmd == "PASS")
			{
				std::cout << "Received password: " << _params.top() << std::endl;
				std::cout << "Params size: " << _params.size() << std::endl;
				if (_params.size() == 1)
				{
					if (_params.top() == _password)
					{
						_authenticatedClients[clientFd] = true;
						const std::string welcomeMessage = "Password accepted. You are now connected.\n";
						send(clientFd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
						std::cout << "Client authenticated." << std::endl;
						// _params.pop();
					}
					else
					{
						const std::string errorMessage = "Invalid password. Connection will be closed.\n";
						send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
						cleanupClient(clientFd);
						// _params.pop();
					}
				}
			}
			else
			{
				const std::string errorMessage = "Invalid command. Connection will be closed.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
				cleanupClient(clientFd);
			}
		}
		if(_authenticatedClients[clientFd])
		{
			if (_cmd == "NICK")
			{
				if (_params.size() != 0 && client.getNickName().empty())
				{
					client.setNickName(_params.top());
					const std::string welcomeNick = "Welcome, " + client.getNickName() + "!\n";
					send(clientFd, welcomeNick.c_str(), welcomeNick.size(), 0);
					// _params.pop();
				}
				else
				{
					const std::string errorMessage = "Invalid nickname. Connection will be closed.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					cleanupClient(clientFd);
				}
			}
			else if (_cmd == "SET")
			{
				if (_params.size() != 2)
				{
					client.setNickName(_params.top());
					const std::string welcomeNick = "Welcome, " + client.getNickName() + "!\n";
					send(clientFd, welcomeNick.c_str(), welcomeNick.size(), 0);
					// _params.pop();
				}
				else
				{
					const std::string errorMessage = "Invalid parameters. Connection will be closed.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					cleanupClient(clientFd);
				}
			}

			//TESTING
			else if (_cmd == "PRIVMSG")
			{
				if (_params.size() < 2)
				{
					const std::string errorMessage = "Usage: PRIVMSG <nickname> <message>\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
				}
				else
				{
					std::string message = _params.top();  // Get the actual message
					std::cout << "message: " << message << std::endl;
					_params.pop();
					std::string recipientNick = _params.top();  // Get recipient's nickname
					std::cout << "recipient nick: " << recipientNick << std::endl;
					_params.pop();

					std::string senderNick = _mapClients[clientFd].getNickName();
					std::string formattedMessage = senderNick + ": " + message + "\n";

					for (std::map<int, Client>::iterator it = _mapClients.begin(); it != _mapClients.end(); ++it)
					{
						// Access the Client object using it->second
						if (it->second.getNickName() == recipientNick)
						{
							// Client with the desired nickname found
							std::cout << "Client with nickname " << recipientNick << " found. Client FD: " << it->first << std::endl;
							send(it->first, formattedMessage.c_str(), formattedMessage.size(), 0);
							break;  // Exit loop once the client is found
						}
					}
				}
			}



			else if (_cmd.empty() && _params.empty())
			{
				return ;
			}
			else
			{
				std::cout << "Message from " << client.getNickName() << ": " << receivedMessage << std::endl;
				if (receivedMessage == "close") {
					std::cout << "Received close message. Shutting down client." << std::endl;
					cleanupClient(clientFd);
				} else if (receivedMessage == "shutdown") {
					std::cout << "Received shutdown message. Shutting down server." << std::endl;
					cleanup();
					exit(0);
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
