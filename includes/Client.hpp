
#pragma once

# include <iostream>
# include <string>
# include <vector>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <sys/select.h>
// # include <stdlib.h>
# include <cstdlib> // substitui a biblioteca stdlib.h
# include <poll.h>
# include <map>

# include <set>


class Client
{
	public:
		Client();
		~Client();

		// getters
		int			getClientFd() const;
		std::string	getNickName() const;
		std::string	getUserName() const;
		std::string	getRealName() const;
		bool		getAuthenticated() const;

		// setters
		void		setClientFd(int clientFd);
		void		setNickName(const std::string &nickName);
		void		setAuthenticated(bool authenticated);
		void		setClientAddress(sockaddr_in clientAddress);
		void		setUserName(const std::string &userName);
		void		setRealName(const std::string &realName);


	private:
		int			_clientFd;
		std::string _userName;
		std::string	_nickName;
		std::string	_realName;
		bool		_authenticated;
};
