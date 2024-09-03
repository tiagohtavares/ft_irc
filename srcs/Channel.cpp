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
	setTopicMode(false);
	setPasswordMode(false);
	setOperatorMode(false);
	setLimitMode(false);
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

void	Channel::setOperator(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _operators.find(client.getClientFd()) == _operators.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_operators.insert(std::make_pair(client.getClientFd(), &client));
	}
}

void	Channel::setBanned(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		// _banned.insert(std::make_pair(client.getClientFd(), &client));
		_banned.insert(client.getClientFd());
	}
}

void	Channel::setInvited(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _invited.find(client.getClientFd()) == _invited.end())
	{
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

std::set<int>	Channel::getBanned() const
{
	return _banned;
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

void	Channel::insertBanned(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_banned.insert(client.getClientFd());
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
			std::string message = "You have been removed from channel " + getChannelName() + ".\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
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

bool	Channel::isOperator(const Client& client) const
{
	return _operators.find(client.getClientFd()) != _operators.end();
}

bool	Channel::isMember(const Client& client) const
{
	return _members.find(client.getClientFd()) != _members.end();
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

bool	Channel::isBanned(const Client& client) const
{
	return _banned.find(client.getClientFd()) != _banned.end();
}

bool	Channel::isInvited(const Client& client) const
{
	return _invited.find(client.getClientFd()) != _invited.end();
}

/************************************************************************************/
/*										Lists										*/
/************************************************************************************/

void Channel::memberList(int clientFd) const
{
	std::map<int, Client*>::const_iterator itOps = _operators.begin();
	while (itOps != _operators.end())
	{
		const std::string operatos = "@" + itOps->second->getNickName() + "\n";
		send(clientFd, operatos.c_str(), operatos.size(), 0);
		itOps++;
	}
	std::map<int, Client*>::const_iterator it = _members.begin();
	while (it != _members.end())
	{
		if (_operators.find(it->first) == _operators.end())
		{
			const std::string members = it->second->getNickName() + "\n";
			send(clientFd, members.c_str(), members.size(), 0);
			it++;
			continue ;
		}
		it++;
	}
}

void	Channel::bannedList() const
{
	std::set<int>::iterator it = _banned.begin();
	while (it != _banned.end())
	{
		std::cout << "!" << *it << std::endl;
		it++;
	}
}

void	Channel::invitedList() const
{
	std::set<int>::iterator it = _invited.begin();
	while (it != _invited.end())
	{
		std::cout << "+" << *it << std::endl;
		it++;
	}
}

