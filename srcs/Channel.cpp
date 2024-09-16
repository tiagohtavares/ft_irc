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
	setLimitMode(false, "");
	setOperatorMode(false);
	setPasswordMode(false, "");
	setTopicMode(false);
	insertMember(client);
	setOperator(client);
	setCreator(client);
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
	for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
	{
		std::string message = "Topic updated: " + getTopic() + "\n";
		send(it->second->getClientFd(), message.c_str(), message.size(), 0);
	}
}

void	Channel::setPassword(std::string &password)
{
	_password = password;
	for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
	{
		std::string message = "Password updated: " + getPassword() + ". Only users with the correct password can join the channel now.\n";
		send(it->second->getClientFd(), message.c_str(), message.size(), 0);
	}
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
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = client.getNickName() + " is now an operator of " + getChannelName() + ".\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
}

void	Channel::setOperator(std::string nickname)
{
	if (isMember(nickname) && !isBanned(nickname))
	{
		std::map<int, Client*>::const_iterator it = _members.begin();
		while (it != _members.end())
		{
			if (it->second->getNickName() == nickname && _operators.find(it->second->getClientFd()) == _operators.end())
			{
				int clientFd = it->first;
				_operators.insert(std::make_pair(clientFd, it->second));

				for (it = _members.begin(); it != _members.end(); it++)
				{
					std::string message = nickname + " is now an operator of " + getChannelName() + ".\n";
					send(it->second->getClientFd(), message.c_str(), message.size(), 0);
				}
				return ;
			}
			it++;
		}
	}
}

void	Channel::setCreator(Client &client)
{
	_creator.insert(std::make_pair(client.getClientFd(), &client));
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

void	Channel::setCreator(std::string nickname)
{
	std::cout << "[serCreator] nickname: " << nickname << std::endl;
	if (isMember(nickname) && !isBanned(nickname))
	{
		std::map<int, Client*>::const_iterator it = _members.begin();
		while (it != _members.end())
		{
			if (it->second->getNickName() == nickname && _creator.find(it->second->getClientFd()) == _creator.end())
			{
				int clientFd = it->first;
				_creator.insert(std::make_pair(clientFd, it->second));

				for (it = _members.begin(); it != _members.end(); it++)
				{
					std::string message = nickname + " is now an operator of " + getChannelName() + ".\n";
					send(it->second->getClientFd(), message.c_str(), message.size(), 0);
				}
				return ;
			}
			it++;
		}
	}
}

void	Channel::setInvitedMode(bool status)
{
	_inviteMode = status;
	if (_inviteMode == true)
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Only invited users can join the channel now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
	else
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Any user can join the channel now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
}

void	Channel::setTopicMode(bool status)
{
	_topicMode = status;
	if (_topicMode == true)
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Only operators can change the topic now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
	else
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Any members can change the topic now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
}

void	Channel::setPasswordMode(bool status, std::string password)
{
	_passwordMode = status;
	if (_passwordMode == true)
	{
		setPassword(password);
	}
	else
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Any user can join the channel without a password now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
}

void	Channel::setOperatorMode(bool status)
{
	_operatorMode = status;
	if (_operatorMode == true)
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Only operators can change channel modes now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
	else
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Any members can change channel modes now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
}

void	Channel::setLimitMode(bool status, std::string limit)
{
	_limitMode = status;
	if (_limitMode == true)
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Member limit mode has been enabled. Only " + limit + " users can join the channel now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
	else
	{
		for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = "Member limit mode has been disabled. Any user can join the channel now.\n";
			send(it->second->getClientFd(), message.c_str(), message.size(), 0);
		}
	}
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

std::map<int, Client*>	Channel::getCreator() const
{
	return _creator;
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
	for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (it->second->getNickName() != client.getNickName())
		{
			std::string message = client.getNickName() + " joined the " + getChannelName() + ".\n";
			send(it->first, message.c_str(), message.size(), 0);
		}
	}
}

void	Channel::insertOperator(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _operators.find(client.getClientFd()) == _operators.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_operators.insert(std::make_pair(client.getClientFd(), &client));
		for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
		{
			std::string message = client.getNickName() + " is now an operator of " + getChannelName() + ".\n";
			send(it->first, message.c_str(), message.size(), 0);
		}
	}
}

void	Channel::insertCreator(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _creator.find(client.getClientFd()) == _creator.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_creator.insert(std::make_pair(client.getClientFd(), &client));
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
		if (getMembers().size() == 0)
		{
			return;
		}
		for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); it++)
		{
			std::string message = client.getNickName() + " has been removed from channel " + getChannelName() + ".\n";
			send(it->first, message.c_str(), message.size(), 0);
		}
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
			if (getMembers().size() == 0)
			{
				return;
			}
			for (it = _members.begin(); it != _members.end(); it++)
			{
				std::string message = nickname + " has been removed from channel " + getChannelName() + ".\n";
				send(it->first, message.c_str(), message.size(), 0);
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
		for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
		{
			std::string message = client.getNickName() + " is no longer an opetator of " + getChannelName() + ".\n";
			send(it->first, message.c_str(), message.size(), 0);
		}
	}
}

void Channel::removeOperator(std::string nickname)
{
	std::map<int, Client*>::iterator it = _operators.begin();
	while (it != _operators.end())
	{
		if (it->second->getNickName() == nickname)
		{
			_operators.erase(it);
			for (it = _members.begin(); it != _members.end(); ++it)
			{
				std::string message = nickname + " is no longer an operator of " + getChannelName() + ".\n";
				send(it->first, message.c_str(), message.size(), 0);
			}
			return;
		}
		++it;
	}
}

void	Channel::removeCreator(Client &client)
{
	if (_creator.find(client.getClientFd()) != _creator.end())
	{
		_creator.erase(client.getClientFd());
		for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
		{
			std::string message = client.getNickName() + " is no longer an creator of " + getChannelName() + ".\n";
			send(it->first, message.c_str(), message.size(), 0);
		}
	}
}

void	Channel::removeBanned(Client &client)
{
	if (_banned.find(client.getClientFd()) != _banned.end())
	{
		_banned.erase(client.getClientFd());
		for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
		{
			std::string message = client.getNickName() + " is no longer an banned of " + getChannelName() + ".\n";
			send(it->first, message.c_str(), message.size(), 0);
		}
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
	std::map<int, Client*>::const_iterator it = _operators.begin();
	while (it != _operators.end())
	{
		if (it->second->getNickName() == client.getNickName())
		{
			return true;
		}
		it++;
	}
	return false;
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

bool	Channel::isCreator(const Client& client) const
{
	return _creator.find(client.getClientFd()) != _creator.end();
}

bool	Channel::isCreator(std::string nickname) const
{
	std::map<int, Client*>::const_iterator it = _creator.begin();
	while (it != _creator.end())
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

std::string Channel::memberList() const
{
	std::string nicknames;
	std::map<int, Client*>::const_iterator itOps = _operators.begin();
	while (itOps != _operators.end() && itOps->second->getModeInvisible() == false)
	{
		// const std::string operatos = "@" + itOps->second->getNickName() + "\n";
		const std::string operatos = "@" + itOps->second->getNickName() + "\n";
		// send(clientFd, operatos.c_str(), operatos.size(), 0);
		itOps++;
		if (!nicknames.empty())
				nicknames += " ";
		nicknames += operatos;
	}
	std::map<int, Client*>::const_iterator it = _members.begin();
	while (it != _members.end())
	{
		if (_operators.find(it->first) == _operators.end() && it->second->getModeInvisible() == false)
		{
			const std::string members = it->second->getNickName() + "\n";
			// send(clientFd, members.c_str(), members.size(), 0);
			it++;
			if (!nicknames.empty())
				nicknames += " ";
			nicknames += members;
			continue ;
		}
		// if (!nicknames.empty())
		// 		nicknames += " ";
		// 	nicknames += members;
		it++;
	}

	return nicknames;
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

int Channel::getUsersCount() const
{
    return _members.size();
}

void	Channel::sendMessageToMembers(const std::string &message) const
{
	for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); it++)
	{
		send(it->first, message.c_str(), message.size(), 0);
	}
}
