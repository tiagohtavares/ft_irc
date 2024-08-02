/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttavares <ttavares@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 16:29:14 by ttavares          #+#    #+#             */
/*   Updated: 2024/08/01 16:29:14 by ttavares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _password(password), _server_fd(-1), _client_fd(-1)
{
}


Server::~Server()
{
	close(_server_fd);
}

void Server::run()
{
    // Create a socket
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1)
    {
        std::cerr << "Failed to create socket! Error: "<< std::endl;
        return;
    }

    // Set the socket to non-blocking mode
    int flags = fcntl(_server_fd, F_GETFL, 0);
    if (flags == -1 || fcntl(_server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Failed to set non-blocking mode! Error: "<< std::endl;
        close(_server_fd);
        return;
    }

    // Specify socket address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(_port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(_server_fd, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Binding failed! Error: "<< std::endl;
        close(_server_fd);
        return;
    }

    // Listen
    if (listen(_server_fd, 5) == -1)
    {
        std::cerr << "Listening failed! Error: "<< std::endl;
        close(_server_fd);
        return;
    }

    std::cout << "Server is running and waiting for connections..." << std::endl << std::flush;

    std::vector<struct pollfd> pollfds;
    struct pollfd serverPollFd;
    serverPollFd.fd = _server_fd;
    serverPollFd.events = POLLIN; // Monitor for incoming connections
    serverPollFd.revents = 0;
    pollfds.push_back(serverPollFd);

	bool running = true;

    while (running)
    {
        // Poll for events
        int pollResult = poll(pollfds.data(), pollfds.size(), -1); // Timeout -1 means wait indefinitely
        if (pollResult == -1)
        {
            std::cerr << "Poll failed! Error: "<< std::endl;
            break;
        }

        // Check if there's activity on the server socket
        if (pollfds[0].revents & POLLIN)
        {
            sockaddr_in clientAddress;
            socklen_t clientSize = sizeof(clientAddress);
            int clientFd = accept(_server_fd, (sockaddr*)&clientAddress, &clientSize);
            if (clientFd == -1)
            {
                std::cerr << "Failed to accept connection! Error: "<< std::endl;
                continue;
            }

            // Set the new socket to non-blocking mode
            int flags = fcntl(clientFd, F_GETFL, 0);
            if (flags == -1 || fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) == -1)
            {
                std::cerr << "Failed to set non-blocking mode for client socket! Error: "<< std::endl;
                close(clientFd);
                continue;
            }

            // Add new client socket to pollfds
            struct pollfd clientPollFd;
            clientPollFd.fd = clientFd;
            clientPollFd.events = POLLIN; // Monitor for incoming data
            clientPollFd.revents = 0;
            pollfds.push_back(clientPollFd);

            std::cout << "Client connected from " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl << std::flush;
        }

        // Check each client socket for data
        for (size_t i = 1; i < pollfds.size(); ++i)
        {
            if (pollfds[i].revents & POLLIN)
            {
                char buffer[4096];
                memset(buffer, 0, sizeof(buffer));
                int bytesReceived = recv(pollfds[i].fd, buffer, sizeof(buffer), 0);
                if (bytesReceived == -1)
                {
                    std::cerr << "Failed to receive message from client! Error: "<< std::endl;
                    close(pollfds[i].fd);
                    pollfds.erase(pollfds.begin() + i);
                    --i; // Adjust index after removal
                }
                else if (bytesReceived == 0)
                {
                    // Client has closed the connection
                    std::cout << "Client disconnected" << std::endl << std::flush;
                    close(pollfds[i].fd);
                    pollfds.erase(pollfds.begin() + i);
                    --i; // Adjust index after removal
                }
                else
                {
                    // Properly terminate the received data
                    std::string receivedMessage(buffer, bytesReceived);
                    receivedMessage.erase(receivedMessage.find_last_not_of("\r\n") + 1);
                    std::cout << "Message from client: " << receivedMessage << std::endl << std::flush;

                    // Check for "close" message to terminate the server
                    if (receivedMessage == "close") {
                        std::cout << "Received close message. Shutting down server." << std::endl << std::flush;
						running = false;
                        close(pollfds[i].fd);
                        pollfds.erase(pollfds.begin() + i);
                        --i; // Adjust index after removal
                        break; // Exit the for loop to process remaining clients
                    }
                }
            }
        }
    }
    // Close the remaining client sockets
    for (size_t i = 1; i < pollfds.size(); ++i)
	{
        close(pollfds[i].fd);
    }
    pollfds.clear();

    // Close the server socket
    if (_server_fd != -1)
	{
        close(_server_fd);
	}
}
