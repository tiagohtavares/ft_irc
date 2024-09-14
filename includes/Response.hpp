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

// 001    RPL_WELCOME
            //   "Welcome to the Internet Relay Network
            //    <nick>!<user>@<host>"
# define RPL_WELCOME(source)	"001 " + source + " :Welcome to the Internet Relay Network" + "\r\n"

#endif
