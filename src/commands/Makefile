# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jcollon <jcollon@student.42lyon.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/07 18:27:41 by jcollon           #+#    #+#              #
#    Updated: 2023/12/13 17:06:57 by jcollon          ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

SHELL := /bin/bash
NAME := commands.a
CC := c++

SRC_FILE := INVITE.cpp \
			JOIN.cpp \
			KICK.cpp \
			MODE.cpp \
			NICK.cpp \
			PASS.cpp \
			USER.cpp \
			TOPIC.cpp \
			CAP.cpp \
			PRIVMSG.cpp \
			QUIT.cpp \
			PING.cpp \
			PART.cpp \

OBJ_FILE := $(SRC_FILE:.cpp=.o)
OBJ = $(addprefix $(DIR_OBJ)/, $(OBJ_FILE))

DIR_OBJ := .obj
FLAGS := -g3 -MMD -MP -Wall -Wextra -Werror -std=c++98

INCLUDE := -I. -I../../include
RELINK := Makefile 

# COLORS
GOOD_TEXT := \033[32m
BAD_TEXT := \033[31m
CLEAN_TEXT := \033[36m
INFO_TEXT := \033[35m
ERASE := \033[2K\r\033[1A
RESET := \033[0m



all: $(NAME)
$(NAME): $(DIR_OBJ) $(OBJ)
	echo -e "$(GOOD_TEXT)✅ Making $(NAME:.a=)$(BAD_TEXT)"
	ar rc $(NAME) $(OBJ)
	printf "$(RESET)"



$(DIR_OBJ):
	mkdir -p $@

$(DIR_OBJ)/%.o: %.cpp $(RELINK)
	echo -e "$(GOOD_TEXT)⏳ Making $(NAME:.a=)"
	printf "$(INFO_TEXT) ➥ Making $(RESET)$@ $(BAD_TEXT)"
	$(CC) $(FLAGS) -o $@ -c $< $(INCLUDE)
	if [[ "$(MAKEFLAGS)" = *"--jobserver-auth"* ]]; then \
		printf "\n"; \
	else \
		printf "$(ERASE)"; \
	fi

clean:
	if [[ " $(MAKECMDGOALS)" = *" $@"* ]]; then \
		echo -e "$(CLEAN_TEXT)🧹 Cleaning $(NAME:.a=)$(RESET)"; \
	fi
	rm -rf $(DIR_OBJ)
fclean: clean
	echo -e "$(CLEAN_TEXT)🧹 FCleaning $(NAME:.a=)$(RESET)"
	rm -f $(NAME)
re: fclean all

.PHONY: all clean fclean re
.SILENT:
-include $(OBJ:.o=.d)
