#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <vector>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <sys/select.h>
# include <cstdlib>
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
		std::string	getClientAddress() const;
		std::string	getRealName() const;
		bool		getAuthenticated() const;
		bool 		getModeInvisible() const;

		// setters
		void		setClientFd(int clientFd);
		void		setNickName(const std::string &nickName);
		void		setAuthenticated(bool authenticated);
		void		setUserName(const std::string &userName);
		void		setClientAddress(const std::string &clientAddress);
		void		setRealName(const std::string &realName);
		void 		setModeInvisible(bool modeInvisible);

	private:
		int			_clientFd;
		std::string	_nickName;
		std::string _userName;
		std::string _clientAddress;
		std::string	_realName;
		bool		_authenticated;
		bool		_modeInvisible;
};

#endif
