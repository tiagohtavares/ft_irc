#include "../includes/Client.hpp"

//-------------------------Contrutor

Client::Client() :
	_clientFd(-1),
	_nickName(""),
	_authenticated(false),
	_modeInvisible(false)
	{
	}


//-----------------------Destructor
Client::~Client()
{
}


//-------------------------Getters

int Client::getClientFd() const
{
	return _clientFd;
}

std::string Client::getNickName() const
{
	return _nickName;
}

std::string Client::getUserName() const
{
	return _userName;
}

std::string Client::getRealName() const
{
	return _realName;
}

bool Client::getAuthenticated() const
{
	return _authenticated;
}

bool Client::getModeInvisible() const
{
	return _modeInvisible;
}

//------------------------------Setters

void Client::setNickName(const std::string &nickName)
{
	_nickName = nickName;
}

void Client::setAuthenticated(bool authenticated)
{
	_authenticated = authenticated;
}

void Client::setUserName(const std::string &userName)
{
	_userName = userName;
}

void Client::setRealName(const std::string &realName)
{
	_realName = realName;
}

void Client::setClientFd(int clientFd)
{
	_clientFd = clientFd;
}


void Client::setModeInvisible(bool modeInvisible)
{
	_modeInvisible = modeInvisible;
}




