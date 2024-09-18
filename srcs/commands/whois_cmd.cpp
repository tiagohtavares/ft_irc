#include "../../includes/Server.hpp"

void	Server::whois_cmd(Client &client, std::vector<std::string> params)
{
	if (params.size() == 1)
	{
		if (!isNicknameInUse(params.front()))
		{
			sendMessage(client.getClientFd(), "No such nick.\n");
			return;
		}
		std::string nickname = params.front(); params.pop_back();
		std::map<int, Client>::iterator it = _mapClients.begin();
		while (it != _mapClients.end())
		{
			if (it->second.getNickName() == nickname)
			{
				std::string whois = "[" + it->second.getNickName() + "] (" + it->second.getUserName() + "@" + it->second.getClientAddress() + "): " + it->second.getRealName() + "\n";
				sendMessage(client.getClientFd(), whois);
				std::string end = "[" + it->second.getNickName() + "] End of WHOIS list.\n";
				sendMessage(client.getClientFd(), end);
				return;
			}
			it++;
		}
	}
	else
	{
		sendMessage(client.getClientFd(), "Invalid command.\n");
		return;
	}
}
