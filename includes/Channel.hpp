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
		Channel();
		Channel(std::string &name, Client &creator);
		~Channel();

		void	setTopic(std::string &topic);
		void	setPassword(std::string &password);
		void	setOperator(Client &client);

		std::string getChannelName() const;
		std::string getTopic() const;
		std::string getPassword() const;
		std::set<int> getMembers() const;

		void	insertMember(Client &client);
		void	removeMember(Client &client);

	private:

		std::string					_channelName; // Input in constructor
		std::string					_topic; // Input in setTopic
		std::string					_password; // Input in setPassword
		std::map<int, std::string>	_nicknames;
		std::set<int>				_members;
		bool						_online;
		std::set<int>				_operators; // The first operator is the creator of the channel
		std::set<int>				_banned; // Set of banned clients (setBanned / removeBanned)

		bool						_inviteOnly; // Set in setMode
		std::set<int>				_invited; // Set of invited clients (setInvited / removeInvited)

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

/*
Como está agora:

O CLIENT faz JOIN em um CHANNEL, e consegue faz JOIN em outro channel mesmo antes de fazer PART do primeiro.
*/

/*
COLOCAR UM BOT PARA FICAR ENVIANDO MENSAGENS PARA O CANAL
*/
