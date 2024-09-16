#include "../../includes/Server.hpp"

void Server::join_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.empty())
	{
		sendMessage(clientFd, "Invalid JOIN command. Connection will be closed.\n");
		return;
	}

	long unsigned int i = 0;
	while (i < params.front().size())
	{
		params.front()[i] = std::tolower(static_cast<int>(params.front()[i]));
		i++;
	}

	std::string channelName = params[0];

	// Verifica nome do canal
	if (!isChannelNameValid(params, clientFd))
		return;

	// Comando para sair de todos os canais
	if (params.size() == 1 && params.front().size() == 1 && (params.front()[0] == '0'))
	{
		leaveAllChannels(client);
		sendMessage(clientFd, "You have left all channels.\n");
		return;
	}

	// JOIN em canal sem senha
	if (params.size() == 1)
	{
		joinChannelWithoutPassword(client, clientFd, channelName);
		return;
	}

	// JOIN em canal com senha
	if (params.size() == 2)
	{
		joinChannelWithPassword(client, clientFd, channelName, params[1]);
		return;
	}

	sendMessage(clientFd, "Invalid JOIN command.\n");
}

bool Server::isChannelNameValid(const std::vector<std::string> &params, int clientFd)
{
	if (params.size() == 1 && params.front().size() == 1 && ((params.front()[0] == '#') || (params.front()[0] == '&')))
	{
		sendMessage(clientFd, "/JOIN <#channel_name>\n");
		return false;
	}
	else if (params.size() == 1 && params.front().size() > 1 && ((params.front()[0] == '#' && params.front()[1] == '#') ||
	(params.front()[0] == '&' && params.front()[1] == '&')))
	{
		sendMessage(clientFd, "The channel name cannot start with ## or &&.\n");
		return false;
	}
	else if (params.front().size() > 51)
	{
		sendMessage(clientFd, "The channel name cannot exceed 50 characters.\n");
		return false;
	}

	return true;
}

void Server::joinChannelWithoutPassword(Client &client, int clientFd, const std::string &channelName)
{
	if (isChannelExist(channelName))
	{
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		if (it != _channels.end())
		{
			if (it->second.isMember(client))
			{
				sendMessage(clientFd, buildWelcomeMessage(it->second));
				std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
				sendMessage(clientFd, joinMessage);
			}
			else if (!it->second.getPasswordMode())
			{
				handleChannelJoin(client, clientFd, it->second);
			}
			else
			{
				sendMessage(clientFd, "This channel is password-protected. Please use /JOIN <#channel_name> <channel_password>\n");
			}
		}
	}
	else
	{
		std::string newChannel = channelName;
		createChannel(newChannel, client);
		sendMessage(clientFd, ":" + client.getNickName() + " JOIN " + channelName + "\r\n");
	}
}

void Server::joinChannelWithPassword(Client &client, int clientFd, const std::string &channelName, const std::string &password)
{
	if (isChannelExist(channelName))
	{
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		if (it != _channels.end())
		{
			if (it->second.isMember(client))
			{
				sendMessage(clientFd, buildWelcomeMessage(it->second));
				std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
				sendMessage(clientFd, joinMessage);
			}
			else if (it->second.getPasswordMode() && password == it->second.getPassword())
			{
				handleChannelJoin(client, clientFd, it->second);
			}
			else
			{
				sendMessage(clientFd, "Invalid password.\n");
			}
		}
	}
	else
	{
		std::string channelNameCopy = channelName;
		createChannel(channelNameCopy, client);
		_channels[channelName].setPasswordMode(true, password);
		sendMessage(clientFd, ":" + client.getNickName() + " JOIN " + channelName + "\r\n");
	}
}


void Server::handleChannelJoin(Client &client, int clientFd, Channel &channel)
{
	// Verificar se o canal está cheio ou se o cliente precisa de convite
	if (channel.getLimitMode() && channel.getMembers().size() >= channel.getLimit())
	{
		sendMessage(clientFd, "The channel is full. You cannot join.\n");
		return;
	}
	if (channel.getInviteMode() && !channel.isInvited(client))
	{
		sendMessage(clientFd, "You need to be invited to join this channel.\n");
		return;
	}

	// Inserir cliente no canal
	channel.insertMember(client);
	sendMessage(clientFd, buildWelcomeMessage(channel));
	std::string joinMessage = ":" + client.getNickName() + " JOIN " + channel.getChannelName() + "\r\n";
	sendMessage(clientFd, joinMessage);
}

// void	Server::join_cmd(Client &client, int clientFd, std::vector<std::string> params)
// {
// 	if (params.empty())
// 	{
// 		sendMessage(clientFd, "Invalid JOIN command. Connection will be closed.\n");
// 		return;
// 	}

// 	long unsigned int i = 0;
// 	while (i < params.front().size())
// 	{
// 		params.front()[i] = std::tolower (params.front()[i]);
// 		i++;
// 	}
// 	std::cout << "params.front() = " << params.front() << std::endl;
// 	return;

// 	std::string channelName = params[0];

// 	// Verifica nome do canal
// 	if (!isChannelNameValid(params, clientFd))
// 		return;

// 	// Comando para sair de todos os canais
// 	if (params.size() == 1 && params.front().size() == 1 && (params.front()[0] == '0'))
// 	{
// 		leaveAllChannels(client);
// 		sendMessage(clientFd, "You have left all channels.\n");
// 		return;
// 	}

// 	// JOIN em canal sem senha
// 	if (params.size() == 1)
// 	{
// 		joinChannelWithoutPassword(client, clientFd, channelName);
// 		return;
// 	}

// 	// JOIN em canal com senha
// 	if (params.size() == 2)
// 	{
// 		joinChannelWithPassword(client, clientFd, channelName, params[1]);
// 		return;
// 	}

// 	sendMessage(clientFd, "Invalid JOIN command.\n");
// }

// bool	Server::isChannelNameValid(const std::vector<std::string> &params, int clientFd)
// {
// 	if (params.size() == 1 && params.front().size() == 1 && ((params.front()[0] == '#') || (params.front()[0] == '&')))
// 	{
// 		sendMessage(clientFd, "/JOIN <#channel_name>\n");
// 		return false;
// 	}
// 	else if (params.size() == 1 && params.front().size() > 1 && ((params.front()[0] == '#' && params.front()[1] == '#') ||
// 	(params.front()[0] == '&' && params.front()[1] == '&')))
// 	{
// 		sendMessage(clientFd, "The channel name cannot start with ## or &&.\n");
// 		return false;
// 	}
// 	else if (params.front().size() > 51)
// 	{
// 		sendMessage(clientFd, "The channel name cannot exceed 50 characters.\n");
// 		return false;
// 	}

// 	// long unsigned int i = 0;
// 	// while (i < params.front().size())
// 	// {
// 	// 	params.front()[i] = std::tolower (params.front()[i]);
// 	// 	i++;
// 	// }

// 	return true;
// }

// void	Server::joinChannelWithoutPassword(Client &client, int clientFd, const std::string &channelName)
// {
// 	if (isChannelExist(channelName))
// 	{
// 		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
// 		if (it != _channels.end())
// 		{
// 			if (it->second.isMember(client))
// 			{
// 				sendMessage(clientFd, buildWelcomeMessage(it->second));
// 				std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 				sendMessage(clientFd, joinMessage);
// 			}
// 			else if (!it->second.getPasswordMode())
// 			{
// 				handleChannelJoin(client, clientFd, it->second);
// 			}
// 			else
// 			{
// 				sendMessage(clientFd, "This channel is password-protected. Please use /JOIN <#channel_name> <channel_password>\n");
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// Criar canal se ele não existir
// 		createChannel(channelName, client);
// 		sendMessage(clientFd, ":" + client.getNickName() + " JOIN " + channelName + "\r\n");
// 	}
// }

// void	Server::joinChannelWithPassword(Client &client, int clientFd, const std::string &channelName, const std::string &password)
// {
// 	if (isChannelExist(channelName))
// 	{
// 		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
// 		if (it != _channels.end())
// 		{
// 			if (it->second.isMember(client))
// 			{
// 				sendMessage(clientFd, buildWelcomeMessage(it->second));
// 				std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 				sendMessage(clientFd, joinMessage);
// 			}
// 			else if (it->second.getPasswordMode() && password == it->second.getPassword())
// 			{
// 				handleChannelJoin(client, clientFd, it->second);
// 			}
// 			else
// 			{
// 				sendMessage(clientFd, "Invalid password.\n");
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// Criar canal com senha
// 		createChannel(channelName, client);
// 		_channels[channelName].setPasswordMode(true, password);
// 		sendMessage(clientFd, ":" + client.getNickName() + " JOIN " + channelName + "\r\n");
// 	}
// }

// void	Server::handleChannelJoin(Client &client, int clientFd, Channel &channel)
// {
// 	// Verificar se o canal está cheio ou se o cliente precisa de convite
// 	if (channel.getLimitMode() && channel.getMembers().size() >= channel.getLimit())
// 	{
// 		sendMessage(clientFd, "The channel is full. You cannot join.\n");
// 		return;
// 	}
// 	if (channel.getInviteMode() && !channel.isInvited(client))
// 	{
// 		sendMessage(clientFd, "You need to be invited to join this channel.\n");
// 		return;
// 	}

// 	// Inserir cliente no canal
// 	channel.insertMember(client);
// 	sendMessage(clientFd, buildWelcomeMessage(channel));
// 	std::string joinMessage = ":" + client.getNickName() + " JOIN " + channel.getChannelName() + "\r\n";
// 	sendMessage(clientFd, joinMessage);
// }











// void	Server::join_cmd(Client &client, int clientFd, std::vector<std::string> params)
// {
// 	if ((params.size() == 1) && (params.front().size() == 1) && ((params.front()[0] == '#') || (params.front()[0] == '&')))
// 		sendMessage(clientFd, "/JOIN <#channel_name>\n");
// 	else if (params.size() == 1 && params.front().size() > 1 && ((params.front()[0] == '#' && params.front()[1] == '#') ||
// 	(params.front()[0] == '&' && params.front()[1] == '&')))
// 		sendMessage(clientFd, "The channel name cannot start with # or &.\n");
// 	else if (params.size() == 1 && params.front().size() == 1 && (params.front()[0] == '0'))
// 	{
// 		leaveAllChannels(client);
// 		sendMessage(clientFd, "You have left all channels.\n");
// 	}
// 	else if (params.size() == 1 && params.front().size() > 1 && (params.front()[0] == '#' || params.front()[0] == '&'))
// 	{
// 		long unsigned int i = 0;
// 		while (i < params.front().size())
// 		{
// 			params.front()[i] = std::tolower (params.front()[i]);
// 			i++;
// 		}
// 		if (params.front().size() > 51)
// 			sendMessage(clientFd, "The channel name cannot exceed 50 characters.\n");
// 		else if (isChannelExist(params.front()))
// 		{
// 			std::string channelName = params.front();
// 			std::map<std::string, Channel>::iterator it = _channels.find(channelName);
// 			if (it !=_channels.end())
// 			{
// 				if (it->second.isMember(client))
// 				{
// 					sendMessage(clientFd, buildWelcomeMessage(it->second));
// 					std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 					std::cout << joinMessage << std::endl;
// 					sendMessage(clientFd, joinMessage);
// 				}
// 				else if (_channels[it->second.getChannelName()].getPasswordMode() == false)
// 				{
// 					if (_channels[params[0]].getLimitMode() == true)
// 					{
// 						if (_channels[params[0]].getMembers().size() >= _channels[params[0]].getLimit())
// 						{
// 							sendMessage(clientFd, "The channel is full. You cannot join.\n");
// 							return;
// 						}
// 						else
// 						{
// 							if (_channels[it->second.getChannelName()].getInviteMode() == true && _channels[params[0]].isInvited(client))
// 							{
// 								_channels[it->second.getChannelName()].insertMember(client);
// 								sendMessage(clientFd, buildWelcomeMessage(it->second));
// 								std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 								sendMessage(clientFd, joinMessage);
// 							}
// 							else if (_channels[it->second.getChannelName()].getInviteMode() == true && !_channels[params[0]].isInvited(client))
// 							{
// 								sendMessage(clientFd, "You need to be invited to join this channel.\n");
// 								return;
// 							}
// 							else if (_channels[it->second.getChannelName()].getInviteMode() == false)
// 							{
// 								_channels[it->second.getChannelName()].insertMember(client);
// 								sendMessage(clientFd, buildWelcomeMessage(it->second));
// 								std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 								sendMessage(clientFd, joinMessage);
// 							}
// 						}
// 					}
// 					else
// 					{
// 						if (_channels[it->second.getChannelName()].getInviteMode() == true && _channels[params[0]].isInvited(client))
// 						{
// 							_channels[it->second.getChannelName()].insertMember(client);
// 							sendMessage(clientFd, buildWelcomeMessage(it->second));
// 							std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 							sendMessage(clientFd, joinMessage);
// 						}
// 						else if (_channels[it->second.getChannelName()].getInviteMode() == true && !_channels[params[0]].isInvited(client))
// 						{
// 							sendMessage(clientFd, "You need to be invited to join this channel.\n");
// 							return;
// 						}
// 						else if (_channels[it->second.getChannelName()].getInviteMode() == false)
// 						{
// 							_channels[it->second.getChannelName()].insertMember(client);
// 							sendMessage(clientFd, buildWelcomeMessage(it->second));
// 							std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 							sendMessage(clientFd, joinMessage);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sendMessage(clientFd, "This channel is password-protected. Please enter the password. Please use /JOIN <#channel_name> <channel_password>\n");
// 				}
// 			}
// 		}
// 		else
// 		{
// 			createChannel(params.front(), client);
// 			std::string channelName = params.front();
// 			std::map<std::string, Channel>::iterator it = _channels.find(channelName);
// 			std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 			sendMessage(clientFd, joinMessage);
// 		}
// 		params.pop_back();
// 	}

// 	else if ((params.size() == 2) && (params[0].size() == 1) && ((params[0][0] == '#') || (params[0][0] == '&')))
// 		sendMessage(clientFd, "/JOIN <#channel_name> <channel_password>\n");
// 	else if (params.size() == 2 && params[0].size() > 1 && ((params[0][0] == '#' && params[0][1] == '#') ||
// 	(params[0][0] == '&' && params[0][1] == '&')))
// 		sendMessage(clientFd, "The channel name cannot start with # or &.\n");
// 	else if (params.size() == 2 && params[0].size() > 1 && (params[0][0] == '#' || params[0][0] == '&'))
// 	{
// 		long unsigned int i = 0;
// 		while (i < params.front().size())
// 		{
// 			params.front()[i] = std::tolower (params.front()[i]);
// 			i++;
// 		}
// 		if (params[0].size() > 51)
// 		{
// 			sendMessage(clientFd, "The channel name cannot exceed 50 characters.\n");
// 			return;
// 		}
// 		if (isChannelExist(params[0]))
// 		{
// 			std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
// 			if (it !=_channels.end())
// 			{
// 				if (it->second.isMember(client))
// 				{
// 					sendMessage(clientFd, buildWelcomeMessage(it->second));
// 					std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 					std::cout << joinMessage << std::endl;
// 					sendMessage(clientFd, joinMessage);
// 				}
// 				else if (_channels[it->second.getChannelName()].getPasswordMode() == true && params[1]  == _channels[params[0]].getPassword())
// 				{
// 					if (_channels[params[0]].getLimitMode() == true)
// 					{
// 						if (_channels[params[0]].getMembers().size() >= _channels[params[0]].getLimit())
// 						{
// 							sendMessage(clientFd, "The channel is full. You cannot join.\n");
// 							return;
// 						}
// 						else
// 						{
// 							if (_channels[it->second.getChannelName()].getInviteMode() == true && _channels[params[0]].isInvited(client))
// 							{
// 								_channels[it->second.getChannelName()].insertMember(client);
// 								sendMessage(clientFd, buildWelcomeMessage(it->second));
// 								std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 								sendMessage(clientFd, joinMessage);
// 							}
// 							else if (_channels[it->second.getChannelName()].getInviteMode() == true && !_channels[params[0]].isInvited(client))
// 							{
// 								sendMessage(clientFd, "You need to be invited to join this channel.\n");
// 								return;
// 							}
// 							else if (_channels[it->second.getChannelName()].getInviteMode() == false)
// 							{
// 								_channels[it->second.getChannelName()].insertMember(client);
// 								sendMessage(clientFd, buildWelcomeMessage(it->second));
// 								std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 								sendMessage(clientFd, joinMessage);
// 							}
// 						}
// 					}
// 					else
// 					{
// 						if (_channels[it->second.getChannelName()].getInviteMode() == true && _channels[params[0]].isInvited(client))
// 						{
// 							_channels[it->second.getChannelName()].insertMember(client);
// 							sendMessage(clientFd, buildWelcomeMessage(it->second));
// 							std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 							sendMessage(clientFd, joinMessage);
// 						}
// 						else if (_channels[it->second.getChannelName()].getInviteMode() == true && !_channels[params[0]].isInvited(client))
// 						{
// 							sendMessage(clientFd, "You need to be invited to join this channel.\n");
// 							return;
// 						}
// 						else if (_channels[it->second.getChannelName()].getInviteMode() == false)
// 						{
// 							_channels[it->second.getChannelName()].insertMember(client);
// 							sendMessage(clientFd, buildWelcomeMessage(it->second));
// 							std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
// 							sendMessage(clientFd, joinMessage);
// 						}
// 					}
// 				}
// 				else
// 				{
// 					sendMessage(clientFd, "Invalid password.\n");
// 				}
// 			}
// 		}
// 		else
// 		{
// 			createChannel(params.front(), client);
// 			_channels[params[0]].setPasswordMode(true, params[1]);
// 			std::string channelName = params.front();
//			 std::map<std::string, Channel>::iterator it = _channels.find(channelName);
//			 std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
//			 sendMessage(clientFd, joinMessage);
// 		}
// 		while (params.size() > 0)
// 			params.pop_back();
// 	}
// 	else
// 	{
// 		sendMessage(clientFd, "Invalid JOIN command. Connection will be closed.\n");
// 	}
// }


