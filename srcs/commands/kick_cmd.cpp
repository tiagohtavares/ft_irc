#include "../../includes/Server.hpp"

void Server::kick_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2)
	{
		sendMessage(clientFd, "Error: Not enough parameters for KICK command. Usage: KICK <channel> <user>\n");
		return;
	}

	std::string channelName = params[0];
	std::string nickName = params[1];

	if (isChannelExist(channelName))
	{
		std::string channelName = params.front();
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		if (it !=_channels.end())
		{
			Channel &channel = it->second;
			if (channel.isOperator(client))
			{
				if (channel.isMember(nickName))
				{
					channel.removeMember(nickName);
					sendMessage(clientFd, "You have kicked " + nickName + " from channel " + channelName + ".\n");
				}
				else
				{
					sendMessage(clientFd, "Error: The user " + nickName + " is not in this channel.\n");
					return;
				}
			}
			else
			{
				sendMessage(clientFd, "Error: You do not have permission to kick users from this channel.\n");
				return;
			}
		}
	}
	else
	{
		sendMessage(clientFd, "Error: Channel " + params[0] + " does not exist.\n");
		return;
	}
}

// 3.2.8 Comando de chute

//       Comando: KICK
//    Parâmetros: <canal> *( "," <canal> ) <usuário> *( "," <usuário> )
//                [<comentário>]

//    O comando KICK pode ser usado para solicitar a remoção forçada de um usuário
//    de um canal. Isso faz com que o <usuário> SEPARE do <canal> por
//    força. Para que a mensagem seja sintaticamente correta, DEVE haver
//    um parâmetro de canal e vários parâmetros de usuário, ou tantos
//    parâmetros do canal, pois há parâmetros do usuário. Se um "comentário" for
//    dado, isso será enviado em vez da mensagem padrão, o apelido
//    do usuário que emite o KICK.

//    O servidor NÃO DEVE enviar mensagens KICK com múltiplos canais ou
//    usuários para clientes. Isso é necessariamente para manter o retrocesso
//    compatibilidade com software cliente antigo.

//  Examples:

//    KICK &Melbourne Matthew         ; Command to kick Matthew from
//                                    &Melbourne

//    KICK #Finnish John :Speaking English
//                                    ; Command to kick John from #Finnish
//                                    using "Speaking English" as the
//                                    reason (comment).

//    :WiZ!jto@tolsun.oulu.fi KICK #Finnish John
//                                    ; KICK message on channel #Finnish
                                //    from WiZ to remove John from channel
