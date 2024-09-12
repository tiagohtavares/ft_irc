#include "../../includes/Server.hpp"

void Server::mode_cmd(Client &client, int clientFd, std::vector<std::string> params)
{
	if(params[1] == "+i" || params[1] == "-i")
		mode_invite_only(client, clientFd, params);
	else if(params[1] == "+l" || params[1] == "-l")
		mode_limit(client, clientFd, params);
	else if(params[1] == "+o" || params[1] == "-o")
		mode_operator(client, clientFd, params);
	else if(params[1] == "+k" || params[1] == "-k")
		mode_password(client, clientFd, params);
	else if(params[1] == "+t" || params[1] == "-t")
		mode_topic(client, clientFd, params);
	else
	{
		sendMessage(clientFd, "Error: Mode not supported.\n");
		return;
	}
}

// SUMMARY OF MODES:
// 	i: Set/remove Invite-only channel
// 		Command:	/mode #channel -/+i
// 		Command:	/invite nick #channel

// 	l: Set/remove the user limit to channel
// 		Command:	/mode #channel -l
// 		Command:	/mode #channel +l limit_quantity

// 	k: Set/remove the channel key (password)
// 		Command:	/mode #channel -k
// 		Command:	/mode #channel +k passwordS
// 		Command:	/join #channel password

// 	o: Give/take channel operator privileges
// 		Command:	/mode #channel -/+o
// 		Command:	/mode #channel -/+o member

// 	t: Set/remove the restrictions of the TOPIC command to channel operators
// 		Command:	/mode #channel -/+t
// 		Command:	/topic #channel topic_text
