/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ttavares <ttavares@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 16:29:17 by ttavares          #+#    #+#             */
/*   Updated: 2024/08/01 16:29:17 by ttavares         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <stdlib.h>
#include <poll.h>

class Server
{
public:
	Server(int port, const std::string& password);
	~Server();
	void run();

private:
	int _port;
	std::string _password;
	int _server_fd; // server file descriptor
	int _client_fd; // client file descriptor

};
