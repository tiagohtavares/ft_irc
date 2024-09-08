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
        std::string errorMessage = "Error: Not enough parameters for INVITE command. Usage: INVITE <nickname> <channel> \n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
    }

    std::string targetNickname = params[0];  // Usuário que será convidado
    std::string channelName = params[1];     // Nome do canal

    // Verificar se o canal existe
    if (!isChannelExist(channelName))
    {
        std::string errorMessage = "Error: Channel " + channelName + " does not exist. \n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
    }

    // Verificar se o usuário convidado existe
    Client* targetClient = findClientByNickname(targetNickname);
    if (targetClient == NULL)
    {
        std::string errorMessage = "Error: User " + targetNickname + " does not exist. \n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        return;
    }

    // Verificar se o usuário que faz o convite é membro do canal
    std::map<std::string, Channel>::iterator itChannel = _channels.find(channelName);
    if (itChannel != _channels.end())
    {
        Channel &channel = itChannel->second;
        if (!channel.isMember(client))
        {
            std::string errorMessage = "Error: You are not a member of the channel " + channelName + ". \n";
            send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
            return;
        }

        // Verificar se o usuário já é membro do canal
        if (channel.isMember(*targetClient))
        {
            std::string errorMessage = "Error: User " + targetNickname + " is already a member of the channel. \n";
            send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
            return;
        }

        // Enviar convite para o usuário convidado
        std::string inviteMessage = "You have been invited to join the channel " + channelName + " by " + client.getNickName() + ".\n";
        send(targetClient->getClientFd(), inviteMessage.c_str(), inviteMessage.size(), 0);
        _channels[params[1]].insertMember(*targetClient);

        // Notificar o cliente que o convite foi enviado
        std::string successMessage = "Invitation sent to " + targetNickname + " for channel " + channelName + ".\n";
        send(clientFd, successMessage.c_str(), successMessage.size(), 0);
    }
    else
    {
        std::string errorMessage = "Error: Channel " + channelName + " does not exist. \n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
    }
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