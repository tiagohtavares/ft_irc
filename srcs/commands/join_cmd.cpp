#include "../../includes/Server.hpp"

void	Server::join_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if ((params.size() == 1) && (params.front().size() == 1) && ((params.front()[0] == '#') || (params.front()[0] == '&')))
		sendMessage(clientFd, "/JOIN <#channel_name>\n");
	else if (params.size() == 1 && params.front().size() > 1 && ((params.front()[0] == '#' && params.front()[1] == '#') ||
	(params.front()[0] == '&' && params.front()[1] == '&')))
		sendMessage(clientFd, "The channel name cannot start with # or &.\n");
	else if (params.size() == 1 && params.front().size() == 1 && (params.front()[0] == '0'))
	{
		leaveAllChannels(client);
		sendMessage(clientFd, "You have left all channels.\n");
	}
	else if (params.size() == 1 && params.front().size() > 1 && (params.front()[0] == '#' || params.front()[0] == '&'))
	{
		if (params.front().size() > 51)
			sendMessage(clientFd, "The channel name cannot exceed 50 characters.\n");
		else if (isChannelExist(params.front()))
		{
			std::string channelName = params.front();
			std::map<std::string, Channel>::iterator it = _channels.find(channelName);
			if (it !=_channels.end())
			{
				Channel &channel = it->second;
				if (channel.isMember(client))
				{
					sendMessage(clientFd, _channels[params.front()].getTopic() + "\n");
				}
				else
				{
					// Handle the case where the channel is invite-only mode
					if (_channels[channelName].getInviteMode() == true)
					{
						if (!_channels[channelName].isInvited(client))
						{
							sendMessage(clientFd, "You need to be invited to join this channel.\n");
							return;
						}
						else
						{
							// Handle the case where the channel have a limit of members
							if (_channels[channelName].getLimitMode() == true)
							{
								if (_channels[channelName].getMembers().size() >= _channels[channelName].getLimit())
								{
									sendMessage(clientFd, "The channel is full. You cannot join.\n");
									return;
								}
								else
								{
									_channels[channelName].insertMember(client);
									sendMessage(clientFd, buildWelcomeMessage(channel));
								}
							}
							else
							{
								_channels[channelName].insertMember(client);
								sendMessage(clientFd, buildWelcomeMessage(channel));
							}
						}
					}
					else
					{
						// Handle the case where the channel is password-protected
						if (_channels[channelName].getPasswordMode() == true)
							sendMessage(clientFd, "This channel is password-protected. Please enter the password. Please use /JOIN <#channel_name> <channel_password>\n");
						else
						{
							if (_channels[channelName].getLimitMode() == true)
							{
								if (_channels[channelName].getMembers().size() >= _channels[channelName].getLimit())
								{
									sendMessage(clientFd, "The channel is full. You cannot join.\n");
									return;
								}
								else
								{
									sendMessage(clientFd, buildWelcomeMessage(channel));
								}
							}
							else
							{
								_channels[channelName].insertMember(client);
								sendMessage(clientFd, buildWelcomeMessage(channel));
							}
						}
					}
				}
			}
		}
		else
		{
			createChannel(params.front(), client);
		}
		params.pop_back();
	}

	// JOIN #channel_name channel_password
	else if ((params.size() == 2) && (params[0].size() == 1) && ((params[0][0] == '#') || (params[0][0] == '&')))
		sendMessage(clientFd, "/JOIN <#channel_name> <channel_password>\n");
	else if (params.size() == 2 && params[0].size() > 1 && ((params[0][0] == '#' && params[0][1] == '#') ||
	(params[0][0] == '&' && params[0][1] == '&')))
		sendMessage(clientFd, "The channel name cannot start with # or &.\n");
	else if (params.size() == 2 && params[0].size() > 1 && (params[0][0] == '#' || params[0][0] == '&'))
	{
		if (params[0].size() > 51)
		{
			sendMessage(clientFd, "The channel name cannot exceed 50 characters.\n");
			return;
		}
		if (isChannelExist(params[0]))
		{
			std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
			if (it !=_channels.end())
			{
				if (params[1]  == _channels[params[0]].getPassword())
				{
					// Handle the case where the channel have a limit of members
					if (_channels[params[0]].getLimitMode() == true)
					{
						if (_channels[params[0]].getMembers().size() >= _channels[params[0]].getLimit())
						{
							sendMessage(clientFd, "The channel is full. You cannot join.\n");
							return;
						}
						else
						{
							_channels[params[0]].insertMember(client);
							sendMessage(clientFd, buildWelcomeMessage(it->second));
						}
					}
					else
					{
						_channels[params[0]].insertMember(client);
						sendMessage(clientFd, buildWelcomeMessage(it->second));
					}
				}
				else
				{
					sendMessage(clientFd, "Invalid password.\n");
				}
			}
		}
		else
		{
			// join #canal pwd
			createChannel(params.front(), client);
			_channels[params[0]].setPasswordMode(true, params[1]);
			// _channels[params[0]].setPassword(params[1]);
		}
		while (params.size() > 0)
			params.pop_back();
	}
	else
	{
		sendMessage(clientFd, "Invalid JOIN command. Connection will be closed.\n");
	}
}

// // 3.2.1 Join message

// //       Command: JOIN
// //    Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] )
// //                / "0"

// //    The JOIN command is used by a user to request to start listening to
// //    the specific channel.  Servers MUST be able to parse arguments in the
// //    form of a list of target, but SHOULD NOT use lists when sendMessageing JOIN
// //    messages to clients.

// //    Once a user has joined a channel, he receives information about
// //    all commands his server receives affecting the channel.  This
// //    includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.
// //    This allows channel members to keep track of the other channel
// //    members, as well as channel modes.

// //    If a JOIN is successful, the user receives a JOIN message as
// //    confirmation and is then sent the channel's topic (using RPL_TOPIC) and
// //    the list of users who are on the channel (using RPL_NAMREPLY), which
// //    MUST include the user joining.

// //    Note that this message accepts a special argument ("0"), which is
// //    a special request to leave all channels the user is currently a member
// //    of.  The server will process this message as if the user had sent
// //    a PART command (See Section 3.2.2) for each channel he is a member
// //    of.

// //   JOIN #foo,&bar fubar            ; Command to join channel #foo using
// //                                    key "fubar" and &bar using no key.

// //    JOIN #foo,#bar fubar,foobar     ; Command to join channel #foo using
// //                                    key "fubar", and channel #bar using
// //                                    key "foobar".

// //    JOIN #foo,#bar                  ; Command to join channels #foo and
// //                                    #bar.

// //    JOIN 0                          ; Leave all currently joined
// //                                    channels.

// //    :WiZ!jto@tolsun.oulu.fi JOIN #Twilight_zone ; JOIN message from WiZ
// //                                    on channel #Twilight_zone



// #include "../../includes/Server.hpp"

// void	Server::join_cmd(Client &client, int clientFd, std::vector<std::string> params)
// {
// 	// Validação inicial para número de parâmetros e formato de nome de canal
// 	if (params.empty())
// 	{
// 		sendMessage(clientFd, "/JOIN <#channel_name>\n");
// 		return;
// 	}

// 	std::string channelName = params.front();

// 	// Validação do nome do canal
// 	if ((channelName[0] != '#' && channelName[0] != '&') || channelName.size() > 50)
// 	{
// 		sendMessage(clientFd, "Invalid channel name or exceeds 50 characters.\n");
// 		return;
// 	}

// 	// JOIN 0: Sair de todos os canais
// 	if (params.size() == 1 && params.front() == "0")
// 	{
// 		leaveAllChannels(client);
// 		sendMessage(clientFd, "You have left all channels.\n");
// 		return;
// 	}

// 	// Verifica se o canal já existe
// 	if (isChannelExist(channelName))
// 	{
// 		handleExistingChannelJoin(client, clientFd, params);
// 	}
// 	else
// 	{
// 		// Criação de um novo canal
// 		createNewChannel(client, clientFd, params);
// 	}
// }

// void Server::handleExistingChannelJoin(Client &client, int clientFd, std::vector<std::string> &params)
// {
// 	std::string channelName = params[0];
// 	Channel &channel = _channels[channelName];

// 	if (channel.isMember(client))
// 	{
// 		// Usuário já é membro, exibe o tópico e lista de membros
// 		sendMessage(clientFd, channel.getTopic() + "\n");
// 		return;
// 	}

// 	// Verificação de modo de convite
// 	if (channel.getInviteMode() && !channel.isInvited(client))
// 	{
// 		sendMessage(clientFd, "You need to be invited to join this channel.\n");
// 		return;
// 	}

// 	// Verificação de limite de membros
// 	if (channel.getLimitMode() && channel.getMembers().size() >= channel.getLimit())
// 	{
// 		sendMessage(clientFd, "The channel is full. You cannot join.\n");
// 		return;
// 	}

// 	// Verificação de proteção por senha
// 	if (channel.getPasswordMode())
// 	{
// 		if (params.size() < 2 || params[1] != channel.getPassword())
// 		{
// 			sendMessage(clientFd, "Invalid password.\n");
// 			return;
// 		}
// 	}

// 	// Insere o membro no canal
// 	channel.insertMember(client);
// 	sendMessage(clientFd, buildWelcomeMessage(channel));
// }

// void Server::createNewChannel(Client &client, int clientFd, std::vector<std::string> &params)
// {
// 	std::string channelName = params[0];
// 	createChannel(channelName, client);

// 	if (params.size() > 1)
// 	{
// 		_channels[channelName].setPasswordMode(true, params[1]);
// 	}
// }

void	Server::leaveAllChannels(Client &client)
{
	std::map<std::string, Channel>::iterator it = _channels.begin();
	while (it != _channels.end())
	{
		if (it->second.isMember(client))
		{
			it->second.removeMember(client);
		}
		++it;
	}
	sendMessage(client.getClientFd(), "You have left all channels.\n");
}
