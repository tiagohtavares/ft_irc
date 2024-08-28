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

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return -1;
	}

	int port = atoi(argv[1]);//	stdlib.h substituida por cstdlib para c++ (atoi)
	std::string password = argv[2];

	Server server(port, password);
	server.run();

	return 0;
}
