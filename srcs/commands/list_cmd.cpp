#include "../../includes/Server.hpp"

#include <sstream>

std::string intToString(int num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}

void Server::listChannels(Client &client)
{
    std::string channelList;

    // Código 321 - RPL_LISTSTART: Indica o início da lista de canais
    channelList += ":server_name 321 " + client.getNickName() + " Channel :Users Name\r\n";

    // Código 322 - RPL_LIST: Nome do canal, número de usuários, tópico (se aplicável)
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        std::string channelName = it->first;
        int numUsers = it->second.getUsersCount();  // Obtém o número de usuários no canal
        std::string topic = it->second.getTopic(); // Obtém o tópico do canal (se houver)

        channelList += ":server_name 322 " + client.getNickName() + " " + channelName + " " + intToString(numUsers) + " :" + topic + "\r\n";
    }

    // Código 323 - RPL_LISTEND: Indica o fim da lista
    channelList += ":server_name 323 " + client.getNickName() + " :End of /LIST\r\n";

    sendMessage(client.getClientFd(), channelList);
}
