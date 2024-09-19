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
	if (params.size() == 1 && params[0].size() == 1 && (params[0][0] == '0'))
	{
		leaveAllChannels(client);
		sendMessage(clientFd, "You have left all channels.\n");
		return;
	}

	if (!isChannelNameValid(params, clientFd))
		return;

	if (params.size() == 1)
	{
		joinChannelWithoutPassword(client, clientFd, channelName);
		return;
	}

	if (params.size() == 2)
	{
		joinChannelWithPassword(client, clientFd, channelName, params[1]);
		return;
	}
	sendMessage(clientFd, "Invalid JOIN command.\n");
}

bool Server::isChannelNameValid(const std::vector<std::string> &params, int clientFd)
{
	if (params.size() == 1)
	{
		if (params[0].size() == 1)
		{
			sendMessage(clientFd, "/JOIN <#channel_name>\n");
			return false;
		}
		else if (params[0].size() > 1)
		{
			if ((params[0][0] != '#' && params[0][0] != '&') || (params[0][1] == '#' || params[0][1] == '&'))
			{
				sendMessage(clientFd, "/JOIN <#channel_name>\n");
				return false;
			}
		}
	}
	else if (params[0].size() > 51)
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
		// listar de membros do canal com o putMemberList
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		if (it != _channels.end())
		{
			putMemberList(client, it->second);
		}

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
	putMemberList(client, channel);
}

void Server::putMemberList(Client &client, Channel &channel)
{
	std::string nicknames = channel.memberList();
	std::vector<std::string> nicknamesSplit = split(nicknames, " ");
	for (size_t i = 0; i < nicknamesSplit.size(); ++i)
	{
		std::string namesResponse = ":server 353 " + client.getNickName() + " = " + channel.getChannelName() + " :" + nicknamesSplit[i] + "\r\n";
		send(client.getClientFd(), namesResponse.c_str(), namesResponse.size(), 0);
	}
	std::string endOfNames = ":server 366 " + client.getNickName() + " " + channel.getChannelName() + " :End of /NAMES list.\r\n";
	send(client.getClientFd(), endOfNames.c_str(), endOfNames.size(), 0);
}

// 3.2.1 Join message

//       Command: JOIN
//    Parameters: ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] )
//                / "0"

//    The JOIN command is used by a user to request to start listening to
//    the specific channel.  Servers MUST be able to parse arguments in the
//    form of a list of target, but SHOULD NOT use lists when sendMessageing JOIN
//    messages to clients.

//    Once a user has joined a channel, he receives information about
//    all commands his server receives affecting the channel.  This
//    includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.
//    This allows channel members to keep track of the other channel
//    members, as well as channel modes.

//    If a JOIN is successful, the user receives a JOIN message as
//    confirmation and is then sent the channel's topic (using RPL_TOPIC) and
//    the list of users who are on the channel (using RPL_NAMREPLY), which
//    MUST include the user joining.

//    Note that this message accepts a special argument ("0"), which is
//    a special request to leave all channels the user is currently a member
//    of.  The server will process this message as if the user had sent
//    a PART command (See Section 3.2.2) for each channel he is a member
//    of.

//   JOIN #foo,&bar fubar            ; Command to join channel #foo using
//                                    key "fubar" and &bar using no key.

//    JOIN #foo,#bar fubar,foobar     ; Command to join channel #foo using
//                                    key "fubar", and channel #bar using
//                                    key "foobar".

//    JOIN #foo,#bar                  ; Command to join channels #foo and
//                                    #bar.

//    JOIN 0                          ; Leave all currently joined
//                                    channels.

//    :WiZ!jto@tolsun.oulu.fi JOIN #Twilight_zone ; JOIN message from WiZ
//                                    on channel #Twilight_zone

