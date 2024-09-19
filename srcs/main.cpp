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

#include "../includes/Server.hpp"

bool isNumerical(const char* str)
{
	for (size_t i = 0; i < strlen(str); i++)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return -1;
	}

	if (!isNumerical(argv[1]))
	{
		std::cerr << "Error: Port number must be numeric." << std::endl;
		return -1;
	}

	//Check ports range and numerical.
	int port = atoi(argv[1]);

	if (port < 1 || port > 65535)
	{
		std::cerr << "Error: Port number must be between 1 and 65535." << std::endl;
		return -1;
	}

	std::string password = argv[2];

	if (password.length() > 20)
	{
		std::cerr << "Error: Password is too long. Maximum length is 20 characters." << std::endl;
		return -1;
	}
	Server server(port, password);
	server.run();

	return 0;
}
