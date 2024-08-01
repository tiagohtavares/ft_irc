/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttavares <ttavares@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 16:16:39 by ttavares          #+#    #+#             */
/*   Updated: 2024/08/01 16:16:39 by ttavares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sstream>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Wrong format." << std::endl;
		return -1;
	}

	int port = 0;
	std::istringstream portStream(argv[1]);
	if (!(portStream >> port) || !(portStream.eof())) {
		std::cerr << "Invalid port number: " << argv[1] << std::endl;
		return -1;
	}

	std::string password = argv[2];

	std::cout << "Port number is " << port << std::endl;
	std::cout << "Password is " << password << std::endl;


	int listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == -1)
	{
		std::cerr << "Failed to create socket." << std::endl;
		return -1;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

	if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1)
	{
		std::cerr << "Failed to bind to ip port." << std::endl;
		return -1;
	}

	if (listen(listening, SOMAXCONN) == -1)
	{
		std::cerr << "Cannot listen." << std::endl;
	}

	sockaddr_in client;
	socklen_t clientSize = sizeof(client);
	char host[NI_MAXHOST];
	char svc[NI_MAXSERV];

	int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	if (clientSocket == -1)
	{
		std::cerr << "Client connecting problem" << std::endl;
	}

	close(listening);
	memset(host, 0, NI_MAXHOST);
	memset(svc, 0, NI_MAXSERV);

	int result = getnameinfo((sockaddr*)&client,sizeof(client),host, NI_MAXHOST,svc,NI_MAXSERV,0);

	if(result)
	{
		std::cout << host <<  "connected on: " << svc << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << "connect on: " << ntohs(client.sin_port) << std::endl;
	}

	char buffer[4096];
	while(true)
	{
		memset(buffer, 0, 4096);
		int bytesRecv = recv(clientSocket, buffer, 4096, 0);
		if (bytesRecv == -1)
		{
			std::cerr << "connection issue." << std::endl;
			break;
		}
		if (bytesRecv == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			break;
		}
		std::cout << "Received: " << std::string(buffer, 0, bytesRecv) << std::endl;
		send(clientSocket, buffer, bytesRecv + 1, 0);
	}

	close(clientSocket);
	return 0;
}
