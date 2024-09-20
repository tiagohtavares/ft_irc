#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <cctype>
# include <sstream>
# include <ctime>
# include <cerrno>
# include <cstring>
# include <cstdlib>
# include <exception>

# include "Client.hpp"
# include "Channel.hpp"
# include "Server.hpp"

# define ENDL std::cout << std::endl;

// Server responses
// 001    RPL_WELCOME
            //   "Welcome to the Internet Relay Network
            //    <nick>!<user>@<host>"
# define RPL_WELCOME(source)	"001 " + source + " :Welcome to the Internet Relay Network" + "\r\n"

// 002    RPL_YOURHOST
			//   "Your host is <servername>, running version <ver>"
# define RPL_YOURHOST(source, serverName, version)	"002 " + source + " :Your host is " + serverName + ", running version " + version + "\r\n"

// 003    RPL_CREATED
			//   "This server was created <date>"
# define RPL_CREATED(source, date)	"003 " + source + " :This server was created " + date + "\r\n"

// 004    RPL_MYINFO
			//   "<servername> <version> <available user modes> <available channel modes>"
# define RPL_MYINFO(source, serverName, version, userModes, channelModes)	"004 " + source + " :" + serverName + " " + version + " " + userModes + " " + channelModes + "\r\n"

// Channel responses
// 331    RPL_NOTOPIC
			//   "<channel> :No topic is set"
# define RPL_NOTOPIC(source, channel)	"331 " + source + " " + channel + " :No topic is set" + "\r\n"

// 332    RPL_TOPIC
			//   "<channel> :<topic>"
# define RPL_TOPIC(source, channel, topic)	"332 " + source + " " + channel + " :" + topic + "\r\n"

// 353    RPL_NAMREPLY
			//   "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
# define RPL_NAMREPLY(source, channel, nicks)	"353 " + source + " " + channel + " :" + nicks + "\r\n"

// 366    RPL_ENDOFNAMES
			//   "<channel> :End of /NAMES list"
# define RPL_ENDOFNAMES(source, channel)	"366 " + source + " " + channel + " :End of /NAMES list" + "\r\n"

// 324    RPL_CHANNELMODEIS
			//   "<channel> <mode> <mode params>"
# define RPL_CHANNELMODEIS(source, channel, mode, modeParams)	"324 " + source + " " + channel + " " + mode + " " + modeParams + "\r\n"

// Client responses
// 311    RPL_WHOISUSER
			//   "<nick> <user> <host> * :<real name>"
# define RPL_WHOISUSER(source, nick, user, host, realName)	"311 " + source + " " + nick + " " + user + " " + host + " * :" + realName + "\r\n"

// 312    RPL_WHOISSERVER
			//   "<nick> <server> :<server info>"
# define RPL_WHOISSERVER(source, nick, server, serverInfo)	"312 " + source + " " + nick + " " + server + " :" + serverInfo + "\r\n"

// 313    RPL_WHOISOPERATOR
			//   "<nick> :is an IRC operator"
# define RPL_WHOISOPERATOR(source, nick)	"313 " + source + " " + nick + " :is an IRC operator" + "\r\n"

// 317    RPL_WHOISIDLE
			//   "<nick> <integer> :seconds idle"
# define RPL_WHOISIDLE(source, nick, idle)	"317 " + source + " " + nick + " " + idle + " :seconds idle" + "\r\n"

// 318    RPL_ENDOFWHOIS
			//   "<nick> :End of /WHOIS list"
# define RPL_ENDOFWHOIS(source, nick)	"318 " + source + " " + nick + " :End of /WHOIS list" + "\r\n"

// 319    RPL_WHOISCHANNELS
			//   "<nick> :<channel1> <channel2> ..."
# define RPL_WHOISCHANNELS(source, nick, channels)	"319 " + source + " " + nick + " :" + channels + "\r\n"

#endif
