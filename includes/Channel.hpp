#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <set>
# include <map>
# include "../includes/Client.hpp"

class Channel
{
	public:
		Channel(std::string &name, Client &creator);
		~Channel();

		void	setTopic(const std::string &topic);
		void	setPassword(const std::string &password);
		void	setOperator(Client &client);

		std::string getChannelName() const;
		std::string getTopic() const;
		std::string getPassword() const;

	private:
		Channel();

		std::string					_channelName; // Input in constructor
		std::string					_topic; // Input in setTopic
		std::string					_password; // Input in setPassword
		std::map<int, std::string>	_nicknames;
		std::set<int>				_members;
		std::set<int>				_operators; // The first operator is the creator of the channel
		std::set<int>				_banned; // Set of banned clients (setBanned / removeBanned)

		void	insertMember(Client &client);
		void	removeMember(Client &client);
		void	memberList();

		void	removeOperator(Client &client);
};

#endif

/*
Command list for Channel IRC:
* General commands:
	- JOIN: Join a channel
	- PART: Leave a channel
	- PRIVMSG: Send a message to a channel or user

* Operator commands:
	- KICK: Kick a user from a channel
	- INVITE: Invite a user to a channel
	- TOPIC: Set the topic of a channel
	- MODE: Set the mode of a channel
		- i: Set/remove Invite-only channel
		- t: Set/remove the restrictions of the TOPIC command to channel operators
		- k: Set/remove the channel key (password)
		- o: Give/take channel operator privileges
		- l: Set/remove the user limit to channel

* Other commands:
	- LIST: List all channels (should be implemented in the server)
	- NAMES: List all users in a channel
	- BAN: Ban a user from a channel
	- UNBAN: Unban a user from a channel
	- PRIVMSG: Send a message to a channel or user
*/

