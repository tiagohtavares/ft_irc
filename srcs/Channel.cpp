#include "../includes/Channel.hpp"
/************************************************************************************/
/*							Constructors and destructor								*/
/************************************************************************************/

Channel::Channel() : _channelName("default")
{
}

Channel::Channel(std::string &channelName, Client &client)
{
	setChannelName(channelName);
	setInvitedMode(false);
	setLimitMode(false);
	setOperatorMode(false);
	setPasswordMode(false);
	setTopicMode(false);
	insertMember(client);
	setOperator(client);
	const std::string errorMessage = "Channel " + getChannelName() + " created!\n";
	send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
}

Channel::~Channel()
{
}

/************************************************************************************/
/*										Setters										*/
/************************************************************************************/

void	Channel::setChannelName(std::string &channelName)
{
	std::transform(channelName.begin(), channelName.end(), channelName.begin(), ::tolower);
	_channelName = channelName;
}

void	Channel::setTopic(std::string &topic)
{
	std::string	channelTitle;
	channelTitle = getChannelName() + ": ";
	_topic =  channelTitle + topic;
}

void	Channel::setPassword(std::string &password)
{
	_password = password;
}

void	Channel::setLimit(unsigned int limit)
{
	_limit = limit;
}

void	Channel::setOperator(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _operators.find(client.getClientFd()) == _operators.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_operators.insert(std::make_pair(client.getClientFd(), &client));
	}
}

void	Channel::setOperator(std::string client)
{
	if (isMember(client) && !isBanned(client))
	{
		std::map<int, Client*>::const_iterator it = _members.begin();
		while (it != _members.end())
		{
			if (it->second->getNickName() == client && _operators.find(it->second->getClientFd()) == _operators.end())
			{
				_operators.insert(std::make_pair(it->second->getClientFd(), it->second));
				break;
			}
			it++;
		}
	}
}

void	Channel::setInvited(Client &client)
{
	std::cout << "setInvited called" << std::endl;
	if (_banned.find(client.getClientFd()) == _banned.end() && _invited.find(client.getClientFd()) == _invited.end())
		{
			std::cout << "setInvited 1: " << client.getNickName() << std::endl;
			_invited.insert(client.getClientFd());
		}
}

void	Channel::setInvitedMode(bool status)
{
	_inviteMode = status;
}

void	Channel::setTopicMode(bool status)
{
	_topicMode = status;
}

void	Channel::setPasswordMode(bool status)
{
	_passwordMode = status;
}

void	Channel::setOperatorMode(bool status)
{
	_operatorMode = status;
}

void	Channel::setLimitMode(bool status)
{
	_limitMode = status;
}

/************************************************************************************/
/*										Getters										*/
/************************************************************************************/

std::string	Channel::getChannelName() const
{
	if (_channelName[0] == '#')
	{
		return _channelName;
	}
	else
	{
		return "#" + _channelName;
	}
}

std::string	Channel::getTopic() const
{
		return _topic;
}

std::string	Channel::getPassword() const
{
	return _password;
}

unsigned int	Channel::getLimit() const
{
	return _limit;
}

Client*	Channel::getMember(std::string nickname) const
{
	std::map<int, Client*>::const_iterator it = _members.begin();
	while (it != _members.end())
	{
		if (it->second->getNickName() == nickname)
		{
			return it->second;
		}
		it++;
	}
	return NULL;
}

std::map<int, Client*>	Channel::getMembers() const
{
	return _members;
}

std::map<int, Client*>	Channel::getOperators() const
{
	return _operators;
}

std::set<int>	Channel::getInvited() const
{
	return _invited;
}

bool	Channel::getInviteMode() const
{
	return _inviteMode;
}

bool	Channel::getTopicMode() const
{
	return _topicMode;
}

bool	Channel::getPasswordMode() const
{
	return _passwordMode;
}

bool	Channel::getOperatorMode() const
{
	return _operatorMode;
}

bool	Channel::getLimitMode() const
{
	return _limitMode;
}

/************************************************************************************/
/*										Inserts										*/
/************************************************************************************/

void Channel::insertMember(Client &client)
{
	if (_inviteMode == false && _members.find(client.getClientFd()) == _members.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_members.insert(std::make_pair(client.getClientFd(), &client));
	}
	else if (_inviteMode == true && _invited.find(client.getClientFd()) != _invited.end())
	{
		if (_banned.find(client.getClientFd()) == _banned.end())
		{
			removeBanned(client);
		}
		_members.insert(std::make_pair(client.getClientFd(), &client));
    }
}

void	Channel::insertOperator(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _operators.find(client.getClientFd()) == _operators.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_operators.insert(std::make_pair(client.getClientFd(), &client));
	}
}


void	Channel::insertInvited(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _invited.find(client.getClientFd()) == _invited.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_invited.insert(client.getClientFd());
	}
}

/************************************************************************************/
/*										Removes										*/
/************************************************************************************/

void	Channel::removeMember(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end())
	{
		_members.erase(client.getClientFd());
		std::string message = "You have been removed from channel " + getChannelName() + ".\n";
		send(client.getClientFd(), message.c_str(), message.size(), 0);
	}
}

void	Channel::removeMember(std::string nickname)
{
	std::map<int, Client*>::iterator it = _members.begin();
	while (it != _members.end())
	{
		if (it->second->getNickName() == nickname)
		{
			_members.erase(it);
			std::string message = nickname + " has been removed from channel " + getChannelName() + ".\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);

			for (it = _members.begin(); it != _members.end(); it++)
			{
				std::string message = nickname + " has been removed from channel " + getChannelName() + ".\n";
				send(it->second->getClientFd(), message.c_str(), message.size(), 0);
			}
			return ;
		}
		it++;
	}
}

void	Channel::removeOperator(Client &client)
{
	if (_operators.find(client.getClientFd()) != _operators.end())
	{
		_operators.erase(client.getClientFd());
	}
}

void	Channel::removeOperator(std::string nickname)
{
	std::map<int, Client*>::iterator it = _operators.begin();
	while (it != _operators.end())
	{
		if (it->second->getNickName() == nickname)
		{
			_operators.erase(it);
			std::string message = nickname + " had his operator status removed from the " + getChannelName() + ".\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);

			for (it = _operators.begin(); it != _operators.end(); it++)
			{
				std::string message = nickname + " had his operator status removed from the " + getChannelName() + ".\n";
				send(it->second->getClientFd(), message.c_str(), message.size(), 0);
			}
			return ;
		}
		it++;
	}
}

void	Channel::removeBanned(Client &client)
{
	if (_banned.find(client.getClientFd()) != _banned.end())
	{
		_banned.erase(client.getClientFd());
	}
}

void	Channel::removeInvited(Client &client)
{
	if (_invited.find(client.getClientFd()) != _invited.end())
	{
		_invited.erase(client.getClientFd());
	}
}

/************************************************************************************/
/*									True or False									*/
/************************************************************************************/

bool Channel::isPasswordProtected() const
{
    return !_password.empty();
}


bool	Channel::isOperator(const Client& client) const
{
	return _operators.find(client.getClientFd()) != _operators.end();
}

bool	Channel::isMember(const Client& client) const
{
	std::map<int, Client*>::const_iterator it = _members.begin();
	while (it != _members.end())
	{
		if (it->second->getNickName() == client.getNickName())
		{
			return true;
		}
		it++;
	}
	return false;
}

bool	Channel::isMember(std::string nickname) const
{
	std::map<int, Client*>::const_iterator it = _members.begin();
	while (it != _members.end())
	{
		if (it->second->getNickName() == nickname)
		{
			return true;
		}
		it++;
	}
	return false;
}

bool	Channel::isOperator(std::string nickname) const
{
	std::map<int, Client*>::const_iterator it = _operators.begin();
	while (it != _operators.end())
	{
		if (it->second->getNickName() == nickname)
		{
			return true;
		}
		it++;
	}
	return false;
}

bool	Channel::isBanned(const Client& client) const
{
	return _banned.find(client.getClientFd()) != _banned.end();
}

bool	Channel::isBanned(std::string nickname) const
{
	std::map<int, Client*>::const_iterator it = _banned.begin();
	while (it != _banned.end())
	{
		if (it->second->getNickName() == nickname)
		{
			return true;
		}
		it++;
	}
	return false;
}

bool	Channel::isInvited(const Client& client) const
{
	std::set<int>::const_iterator it = _invited.begin();
	while (it != _invited.end())
	{
		if (*it == client.getClientFd())
		{
			std::cout << "isInvited: TRUE: " << *it << std::endl;
			return true;
		}
		it++;
	}
	std::cout << "isInvited: FALSE" << std::endl;
	return false;
}

/************************************************************************************/
/*										Lists										*/
/************************************************************************************/

void Channel::memberList(int clientFd) const
{
	std::map<int, Client*>::const_iterator itOps = _operators.begin();
	while (itOps != _operators.end() && itOps->second->getModeInvisible() == false)
	{
		// const std::string operatos = "@" + itOps->second->getNickName() + "\n";
		const std::string operatos = itOps->second->getNickName() + "\n";
		send(clientFd, operatos.c_str(), operatos.size(), 0);
		itOps++;
	}
	std::map<int, Client*>::const_iterator it = _members.begin();
	while (it != _members.end())
	{
		if (_operators.find(it->first) == _operators.end() && it->second->getModeInvisible() == false)
		{
			const std::string members = it->second->getNickName() + "\n";
			send(clientFd, members.c_str(), members.size(), 0);
			it++;
			continue ;
		}
		it++;
	}
}


// void Channel::memberList(int clientFd) const
// {
//     // Enviar operadores
//     std::map<int, Client*>::const_iterator itOps = _operators.begin();
//     while (itOps != _operators.end())
//     {
//         if (itOps->second->getModeInvisible() == false)
//         {
//             std::string operatorNick = "@" + itOps->second->getNickName();
//             operatorNick += "\r\n"; // IRC usa \r\n para terminadores de linha

//             if (send(clientFd, operatorNick.c_str(), operatorNick.size(), 0) == -1)
//             {
// 				std::cerr << "Error sending operator list" << std::endl;
//                 return; // Sai da função em caso de erro
//             }
//         }
//         ++itOps;
//     }

//     // Enviar membros não operadores
//     std::map<int, Client*>::const_iterator it = _members.begin();
//     while (it != _members.end())
//     {
//         if (_operators.find(it->first) == _operators.end() && it->second->getModeInvisible() == false)
//         {
//             std::string memberNick = it->second->getNickName();
//             memberNick += "\r\n"; // IRC usa \r\n para terminadores de linha

//             if (send(clientFd, memberNick.c_str(), memberNick.size(), 0) == -1)
//             {

// 				std::cerr << "Error sending member list" << std::endl;
//                 return; // Sai da função em caso de erro
//             }
//         }
//         ++it;
//     }
// }


void	Channel::invitedList() const
{
	std::set<int>::iterator it = _invited.begin();
	while (it != _invited.end())
	{
		std::cout << "+" << *it << std::endl;
		it++;
	}
}

void Channel::broadcastMessage(int sendMessageerFd, const std::string &message)
{
    for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
    {
        int memberFd = it->first;

        // Skip the sendMessageer of the message
        if (memberFd != sendMessageerFd)
        {
            send(memberFd, message.c_str(), message.size(), 0);
        }
    }
}

int Channel::getUsersCount() const
{
    return _members.size();
}
