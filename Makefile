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

# CXX = c++
# CXXFLAGS = -Wall -Wextra -std=c++98
# SRC = srcs/main.cpp srcs/Server.cpp srcs/Client.cpp
# OBJ = $(SRC:.cpp=.o)
# TARGET = ircserv

# all: $(TARGET)

# $(TARGET): $(OBJ)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# clean:
# 	rm -f $(OBJ) $(TARGET)

# .PHONY: all clean


CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98 -Iincludes
NAME = ircserv

SRC_DIR = srcs
OBJ_DIR = objects

SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/Server.cpp \
	   $(SRC_DIR)/Client.cpp \
	   $(SRC_DIR)/Channel.cpp \

OBJS = $(OBJ_DIR)/main.o \
		$(OBJ_DIR)/Server.o \
		$(OBJ_DIR)/Client.o \
		$(OBJ_DIR)/Channel.o \

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
