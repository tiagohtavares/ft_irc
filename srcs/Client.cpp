#include "../includes/Client.hpp"

// Contrutor and Destructor

Client::Client() : _clientFd(-1), _nickName(""), _authenticated(false)
{
	// std::cout << "Client created" << std::endl;
}

Client::~Client()
{
}

// Getters and Setters

int Client::getClientFd() const
{
	return _clientFd;
}

std::string Client::getNickName() const
{
	return _nickName;
}

bool Client::getAuthenticated() const
{
	return _authenticated;
}

void Client::setNickName(const std::string &nickName)
{
	_nickName = nickName;
}

void Client::setAuthenticated(bool authenticated)
{
	_authenticated = authenticated;
}

void Client::setClientFd(int clientFd)
{
	_clientFd = clientFd;
}

// void Client::setClientAddress(sockaddr_in clientAddress)
// {
// 	_clientAddress = clientAddress;
// }
