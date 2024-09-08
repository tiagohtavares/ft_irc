#include "../../includes/Server.hpp"

void Server::kick_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if (params.size() < 2)
	{
		std::string errorMessage = "Error: Not enough parameters for KICK command. Usage: KICK <channel> <user>\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		return;
	}

	if (isChannelExist(params[0]))
	{
		std::string channelName = params.front();
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		if (it !=_channels.end())
		{
			Channel &channel = it->second;
			if (channel.isOperator(client))
			{
				if (channel.isMember(params[1]))
				{
					channel.removeMember(params[1]);
					std::string kickMessage3 = "You have kicked " + params[1] + " from channel " + channelName + ".\n";
					send(clientFd, kickMessage3.c_str(), kickMessage3.size(), 0);
				}
				else
				{
					std::string errorMessage = "Error: The user " + params[1] + " is not in this channel.\n";
					send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
					return;
				}
			}
			else
			{
				std::string errorMessage = "Error: You do not have permission to kick users from this channel.\n";
				send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
				return;
			}
		}
	}
	else
	{
		std::string errorMessage = "Error: Channel " + params[0] + " does not exist.\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
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
