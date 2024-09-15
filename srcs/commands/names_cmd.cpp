#include "../../includes/Server.hpp"

std::vector<std::string> split(const std::string& str, const std::string& delimiter) 
{
    std::vector<std::string> tokens;
    std::string token;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) 
	{
        token = str.substr(start, end - start);
        tokens.push_back(token);
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    // Adicionar o último token
    token = str.substr(start);
    tokens.push_back(token);

    return tokens;
}


void Server::names_cmd(Client &client, int clientFd, std::vector<std::string> params) 
{
	if (params.size() == 1)
	{
		std::string channelName = params[0];
		std::map<std::string, Channel>::iterator it = _channels.find(channelName);
		
		if (it != _channels.end())
		{
			std::string nicknames = it->second.memberList(); // Criar a lista de nicks dos membros do canal
			std::vector<std::string> nicknamesSplit = split(nicknames, " ");
			for(size_t  i = 0; i < nicknamesSplit.size(); ++i)
			{	
				std::string namesResponse = ":server 353 " + client.getNickName() + " = " + channelName + " :" + nicknamesSplit[i] + "\r\n";
				send(clientFd, namesResponse.c_str(), namesResponse.size(), 0);
				std::cout << "nick: "<< nicknamesSplit[i] << "\n";
			}
			// Enviar a mensagem de fim de lista de nomes (código 366)
			std::string endOfNames = ":server 366 " + client.getNickName() + " " + channelName + " :End of /NAMES list.\r\n";
			send(clientFd, endOfNames.c_str(), endOfNames.size(), 0);
		}
		else
		{
			
			sendMessage(clientFd, "403 " + channelName + " :No such channel\n");// Se o canal não existe, enviar mensagem de erro (código 403)
		}
	}
	else
	{
		
		sendMessage(clientFd, "461 NAMES :Not enough parameters\n");// Enviar erro 461 se faltar parâmetros
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


