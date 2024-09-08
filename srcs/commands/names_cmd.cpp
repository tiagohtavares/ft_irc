#include "../../includes/Server.hpp"

void	Server::names_cmd(int clientFd, std::vector<std::string> params)
{
	if (params.size() == 1)
	{
		std::map<std::string, Channel>::iterator it = _channels.find(params[0]);
		if (it != _channels.end())
		{
			_channels[params[0]].memberList(clientFd);
		}
		else
		{
			const std::string errorMessage = "403 " + params[0] + " :No such channel\n";
			send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
		}
	}
	else
	{
		const std::string errorMessage = "461 NAMES :Not enough parameters\n";
		send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
	}
}

// 3.2.5 Mensagem de nomes

//       Comando: NOMES
//    Parâmetros: [ <canal> *( "," <canal> ) [ <alvo> ] ]

//    Ao usar o comando NAMES, um usuário pode listar todos os apelidos que são
//    visível para ele. Para mais detalhes sobre o que é visível e o que não é,
//    veja "Internet Relay Chat: Gerenciamento de Canais" [ IRC-CHAN ]. O
//    O parâmetro <channel> especifica qual(is) canal(ais) deve(m) retornar informações
//    sobre. Não há resposta de erro para nomes de canais ruins.

//    Se nenhum parâmetro <canal> for fornecido, uma lista de todos os canais e seus
//    ocupantes é retornado. No final desta lista, uma lista de usuários que
//    são visíveis, mas não estão em nenhum canal ou não estão em um canal visível
//    estão listados como estando no `canal' "*".

//    Se o parâmetro <target> for especificado, a solicitação será encaminhada para
//    aquele servidor que irá gerar a resposta.

//    Curingas são permitidos no parâmetro <target>.


// Exemplos:

//    NOMES #twilight_zone,#42 ; Comando para listar usuários visíveis em
//                                    #twilight_zone e #42

//    NOMES; Comando para listar todos os visíveis
//                                    canais e usuários


