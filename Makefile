# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ttavares <ttavares@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/01 16:26:22 by ttavares          #+#    #+#              #
#    Updated: 2024/08/01 16:26:22 by ttavares         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


# *==============================================================================
# *VARIABLES
# *==============================================================================

NAME		:=	ircserv
CXX			:=	c++
CXXFLAGS	:=	-Wall -Werror -Wextra -std=c++98 -g -lreadline
HEADERS		:=	./includes/main.hpp ./includes/Client.hpp ./includes/Channel.hpp ./includes/Server.hpp
SRCS		:=	./srcs/main.cpp \
				./srcs/Client.cpp \
				./srcs/Server.cpp \
				./srcs/Channel.cpp \
				./srcs/commands/privmsg_cmd.cpp \
				./srcs/commands/list_cmd.cpp \
				./srcs/commands/topic_cmd.cpp \
				./srcs/commands/nick_cmd.cpp \
				./srcs/commands/user_cmd.cpp \
				./srcs/commands/join_cmd.cpp \
				./srcs/commands/part_cmd.cpp \
				./srcs/commands/quit_cmd.cpp \
				./srcs/commands/pass_cmd.cpp \
				./srcs/commands/kick_cmd.cpp \
				./srcs/commands/names_cmd.cpp \
				./srcs/commands/msg_cmd.cpp \
				./srcs/commands/invite_cmd.cpp \
				./srcs/commands/mode_cmd.cpp \
				./srcs/utils_and_flags/messages.cpp \
				./srcs/utils_and_flags/mode_invite.cpp \
				./srcs/utils_and_flags/mode_key.cpp \
				./srcs/utils_and_flags/mode_limite.cpp \
				./srcs/utils_and_flags/mode_operator.cpp \
				./srcs/utils_and_flags/mode_topic.cpp \


OBJS		:=	$(SRCS:.cpp=.o)

# *==============================================================================
# *COLORS
# *==============================================================================

EOC				:=	"\e[0m"
ITALIC_GRAY		:=	"\e[3;37m"
ITALIC_MAGENTA	:=	"\e[1;3;35m"
ITALIC_GREEN 	:= 	"\e[1;3;32m"

# *==============================================================================
# *DEBUG
# *==============================================================================

ifdef DEBUG
	CFLAGS		+= -g -fsanitize=address
endif

# *==============================================================================
# *ROLES
# *==============================================================================

all:		$(NAME)

%.o:	%.cpp $(HEADERS)
			@echo $(ITALIC_GRAY) Compiling $<$(EOC)
			$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME):	$(OBJS)
			@echo $(ITALIC_GREEN)$(NAME) created! $(EOC)
			$(CXX) $(OBJS) $(CXXFLAGS) -o $@

debug:
	$(CXX) -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address  -o $(NAME) $(SRCS)

clean:
			@rm -rf $(OBJS)
			@echo $(ITALIC_MAGENTA)"Objects deleted"$(EOC)

fclean:		clean
			@rm -f $(NAME)
			@echo $(ITALIC_MAGENTA)$(NAME)" deleted"$(EOC)

re:			fclean all

.PHONY:		all clean fclean re
