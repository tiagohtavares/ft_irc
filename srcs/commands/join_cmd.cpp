#include "../../includes/Server.hpp"

void	Server::join_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if ((params.size() == 1) && (params.front().size() == 1) && ((params.front()[0] == '#') || (params.front()[0] == '&')))
		sendMessage(clientFd, "/JOIN <#channel_name>\n");
	else if (params.size() == 1 && params.front().size() > 1 && ((params.front()[0] == '#' && params.front()[1] == '#') ||
	(params.front()[0] == '&' && params.front()[1] == '&')))
		sendMessage(clientFd, "The channel name cannot start with # or &.\n");
	else if (params.size() == 1 && params.front().size() > 1 && (params.front()[0] == '#' || params.front()[0] == '&'))
	{
		long unsigned int i = 0;
		while (i < params.front().size())
		{
			params.front()[i] = std::tolower (params.front()[i]);
			i++;
		}

		if (params.front().size() > 51)
			sendMessage(clientFd, "The channel name cannot exceed 50 characters.\n");
		else if (isChannelExist(params.front()))
		{
			std::string channelName = params.front();
			std::map<std::string, Channel>::iterator it = _channels.find(channelName);
			if (it !=_channels.end())
			{
				// Channel &channel = it->second;
				if (it->second.isMember(client))
				{
					sendMessage(clientFd, buildWelcomeMessage(it->second));
                    std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
                    std::cout << joinMessage << std::endl;
                    sendMessage(clientFd, joinMessage);
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
									sendMessage(clientFd, buildWelcomeMessage(it->second));
                                    std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
                                    sendMessage(clientFd, joinMessage);
								}
							}
							else
							{
								_channels[channelName].insertMember(client);
								sendMessage(clientFd, buildWelcomeMessage(it->second));
                                std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
                                sendMessage(clientFd, joinMessage);
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
									sendMessage(clientFd, buildWelcomeMessage(it->second));
								}
							}
							else
							{
                                _channels[channelName].insertMember(client);
                                std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
                                sendToChannel(it->second.getChannelName(), joinMessage);
							}
						}
					}
				}
			}
		}
		else
		{
            createChannel(params.front(), client);
            std::string channelName = params.front();
            std::map<std::string, Channel>::iterator it = _channels.find(channelName);
            std::string joinMessage = ":" + client.getNickName() + " JOIN " + it->second.getChannelName() + "\r\n";
            sendMessage(clientFd, joinMessage);

		}
		params.pop_back();
	}
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

