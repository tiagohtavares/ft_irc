#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <cctype>
# include <algorithm>
# include <set>
# include <map>
# include "../includes/Client.hpp"

class Channel
{
	public:
		Channel();
		Channel(std::string &name, Client &creator);
		~Channel();

		void			setChannelName(std::string &channelName);
		void			setTopic(std::string &topic);
		void			setPassword(std::string &password);
		void			setLimit(unsigned int limit);
		void			setOperator(Client &client);
		void			setOperator(std::string client);
		void			setInvitedMode(bool status);
		void			setLimitMode(bool status);
		void			setOperatorMode(bool status);
		void			setPasswordMode(bool status);
		void			setTopicMode(bool status);

		// void			setBanned(Client &client);
		void			setInvited(Client &client);

		std::string		getChannelName() const;
		std::string		getTopic() const;
		std::string		getPassword() const;
		unsigned int	getLimit() const;
		Client*			getMember(std::string nickname) const;
		std::map<int, Client*>	getMembers() const;
		std::map<int, Client*>	getOperators() const;
		// std::set<int>	getBanned() const;
		std::set<int>	getInvited() const;
		bool			getInviteMode() const;
		bool			getTopicMode() const;
		bool			getPasswordMode() const;
		bool			getOperatorMode() const;
		bool			getLimitMode() const;

		void			insertOperator(Client &client);
		void			insertMember(Client &client);
		// void			insertBanned(Client &client);
		void			insertInvited(Client &client);

		void			removeOperator(Client &client);
		void			removeOperator(std::string client);
		void			removeMember(Client &client);
		void			removeMember(std::string nickname);
		void			removeBanned(Client &client);
		void			removeInvited(Client &client);

		bool			isOperator(const Client& client) const;
		bool			isOperator(std::string nickname) const;
		bool			isMember(const Client& client) const;
		bool			isMember(std::string nickname) const;
		bool			isBanned(const Client& client) const;
		bool			isBanned(std::string nickname) const;
		bool			isInvited(const Client& client) const;

		void			memberList(int clientFd) const;
		void			bannedList() const;
		void			invitedList() const;

		bool			isPasswordProtected() const;

		void			broadcastMessage(int senderFd, const std::string &message);

	private:
		std::string					_channelName;
		std::string					_topic;
		std::string					_password;
		unsigned int				_limit;
		std::map<int, Client*>		_members;
		std::map<int, Client*>		_creator;
		std::map<int, Client*>		_operators;
		std::map<int, Client*>		_banned;
		std::set<int>				_invited;
		bool						_inviteMode;// MODE #canal +i (true) or -i (false)
		bool						_limitMode;// MODE +l (true) or -l (false)
		bool						_operatorMode;// MODE +o (true) or -o (false)
		bool						_passwordMode;// MODE +k (true) or -k (false)
		bool						_topicMode;// MODE +t (true) or -t (false)
};

#endif

/*
Command list for Channel IRC:
* General commands:
	- JOIN: Join a channel --> DONE!
	- PART: Leave a channel --> DONE!
	- PRIVMSG: Send a message to a channel or user --> DONE!

* Operator commands:
	- KICK: Kick a user from a channel --> DONE!
	- INVITE: Invite a user to a channel
	- TOPIC: Set the topic of a channel --> DONE!
	- MODE: Set the mode of a channel
		- i: Set/remove Invite-only channel
		- t: Set/remove the restrictions of the TOPIC command to channel operators
		- k: Set/remove the channel key (password)
		- o: Give/take channel operator privileges
		- l: Set/remove the user limit to channel

* Other commands:
	- LIST: List all channels (should be implemented in the server) --> DONE!
	- NAMES: List all users in a channel
	- BAN: Ban a user from a channel
	- UNBAN: Unban a user from a channel
	- PRIVMSG: Send a message to a channel or user --> DONE!
*/
