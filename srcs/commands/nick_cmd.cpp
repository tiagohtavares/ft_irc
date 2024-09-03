#include "../../includes/Server.hpp"

void	Server::nick_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
    if (params.size() != 0) //Lidar com situacao de 2 nicks iguais ------------!!!
    {
        client.setNickName(params.front());
    }
    else
    {
        const std::string errorMessage = "Invalid nickname. Connection will be closed.\n";
        send(clientFd, errorMessage.c_str(), errorMessage.size(), 0);
        cleanupClient(clientFd);
        return;
    }
}