#include "../includes/Channel.hpp"

/************************************************************************************/
/*							Constructors and destructor								*/
/************************************************************************************/

Channel::Channel() : _channelName("default")
{
}

Channel::Channel(std::string &channelName, Client &client) : _channelName(channelName)
{
	insertMember(client);
	setOperator(client);
	const std::string errorMessage = "Channel created!\n";
	send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
}

Channel::~Channel()
{
}

/************************************************************************************/
/*							Setters and getters										*/
/************************************************************************************/

void	Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}

void	Channel::setPassword(const std::string &password)
{
	_password = password;
}

std::string	Channel::getChannelName() const
{
	return _channelName;
}

std::string	Channel::getTopic() const
{
	return _topic;
}

std::string	Channel::getPassword() const
{
	return _password;
}

std::set<int>	Channel::getMembers() const
{
	return _members;
}

/************************************************************************************/
/*								Channel management									*/
/************************************************************************************/

void	Channel::insertMember(Client &client)
{
	if (_password.empty() && _members.find(client.getClientFd()) == _members.end() && _banned.find(client.getClientFd()) == _banned.end())
	{
		_members.insert(client.getClientFd());
		// const std::string errorMessage = "You have joined the channel " + getChannelName() + "!\n";
		// send(client.getClientFd(), errorMessage.c_str(), errorMessage.size(), 0);
	}
}

void	Channel::removeMember(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end())
		_members.erase(client.getClientFd());
}

void	Channel::setOperator(Client &client)
{
	if (_members.find(client.getClientFd()) != _members.end() && _operators.find(client.getClientFd()) == _operators.end())
		_operators.insert(client.getClientFd());
}

void	Channel::removeOperator(Client &client)
{
	if (_operators.find(client.getClientFd()) != _operators.end())
		_operators.erase(client.getClientFd());
}

// // Add for LIST command
// void	Channel::memberList()
// {
// 	std::set<int>::iterator it = _members.begin();
// 	while (it != _members.end())
// 	{
// 		std::cout << *it << std::endl;
// 		it++;
// 	}
// }

/* General commands */
// JOIN
// PART
// PRIVMSG

