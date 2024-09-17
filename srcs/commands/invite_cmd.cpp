#include "../../includes/Server.hpp"

Client* Server::findClientByNickname(const std::string &nickname)
{
    for (std::map<int, Client>::iterator it = _mapClients.begin(); it != _mapClients.end(); ++it)
    {
        if (it->second.getNickName() == nickname)
            return &(it->second);
    }
    return NULL;
}


void Server::invite_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
    if (params.size() != 2)
    {
        sendMessage(clientFd, "Error: Not enough parameters for INVITE command. Usage: INVITE <nickname> <channel> \n");
        return;
    }

    std::string targetNickname = params[0];  // Usuário que será convidado
    std::string channelName = params[1];     // Nome do canal

    // Verificar se o canal existe
    if (!isChannelExist(channelName))
    {
        sendMessage(clientFd, "Error: Channel " + channelName + " does not exist. \n");
        return;
    }

    // Verificar se o usuário convidado existe
    Client* targetClient = findClientByNickname(targetNickname);
    if (targetClient == NULL)
    {
        sendMessage(clientFd, "Error: User " + targetNickname + " does not exist. \n");
        return;
    }

    // Verificar se o usuário que faz o convite é membro do canal
    std::map<std::string, Channel>::iterator itChannel = _channels.find(channelName);
    if (itChannel != _channels.end())
    {
        Channel &channel = itChannel->second;
        if (!channel.isMember(client))
        {
            sendMessage(clientFd, "Error: You are not a member of the channel " + channelName + ". \n");
            return;
        }
        // Verificar se o usuário já é membro do canal
        if (channel.isMember(*targetClient))
        {
            sendMessage(clientFd, "Error: User " + targetNickname + " is already a member of the channel. \n");
            return;
        }
		if (_channels[channelName].getOperatorMode() == true)
		{
			if (_channels[channelName].isOperator(client))
			{
				_channels[channelName].setInvited(*targetClient);
				// Enviar convite para o usuário convidado
				sendMessage(targetClient->getClientFd(), "You have been invited to join the channel " + channelName + " by " + client.getNickName() + ".\n");
				buildWelcomeMessage(channel);

				// Notificar o cliente que o convite foi enviado
				sendMessage(clientFd, "Invitation sent to " + targetNickname + " for channel " + channelName + ".\n");
			}
			else
			{
				sendMessage(clientFd, "Error: You must be an operator to invite users to the channel " + channelName + ". \n");
				return;
			}
		}
		else
		{
			_channels[channelName].setInvited(*targetClient);
			// Enviar convite para o usuário convidado
			sendMessage(targetClient->getClientFd(), "You have been invited to join the channel " + channelName + " by " + client.getNickName() + ".\n");
			buildWelcomeMessage(channel);

			// Notificar o cliente que o convite foi enviado
			sendMessage(clientFd, "Invitation sent to " + targetNickname + " for channel " + channelName + ".\n");
		}
    }
    else
        sendMessage(clientFd, "Error: Channel " + channelName + " does not exist. \n");
}

// 3.2.7 Invite message

//       Command: INVITE
//    Parameters: <nickname> <channel>

//    The INVITE command is used to invite a user to a channel.  The
//    parameter <nickname> is the nickname of the person to be invited to
//    the target channel <channel>.  There is no requirement that the
//    channel the target user is being invited to must exist or be a valid
//    channel.  However, if the channel exists, only members of the channel
//    are allowed to invite other users.  When the channel has invite-only
//    flag set, only channel operators may issue INVITE command.
//
// 	  Only the user inviting and the user being invited will receive
//    notification of the invitation.  Other channel members are not
//    notified.  (This is unlike the MODE changes, and is occasionally the
//    source of trouble for users.)

//    Numeric Replies:

//            ERR_NEEDMOREPARAMS              ERR_NOSUCHNICK
//            ERR_NOTONCHANNEL                ERR_USERONCHANNEL
//            ERR_CHANOPRIVSNEEDED
//            RPL_INVITING                    RPL_AWAY

//    Examples:

//    :Angel!wings@irc.org INVITE Wiz #Dust

//                                    ; Message to WiZ when he has been
//                                    invited by user Angel to channel
//                                    #Dust

//    INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to
//                                    #Twilight_zone
