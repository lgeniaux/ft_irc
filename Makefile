# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jcollon <jcollon@student.42lyon.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/07 17:05:51 by jcollon           #+#    #+#              #
#    Updated: 2023/09/07 21:01:13 by jcollon          ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

NAME := ircserv
SHELL := /bin/bash
CC := c++

SRC_FILE :=	Command.cpp \
			Channel.cpp \
			Server.cpp \
			Client.cpp \
			RFC2812Handler.cpp \
			main.cpp
OBJ = $(addprefix $(DIR_OBJ)/, $(SRC_FILE:.cpp=.o))

DIR_OBJ := .obj
COMMANDS := src/commands/commands.a
FLAGS := -g -MMD -MP -std=c++98 -Wall -Wextra -Werror
INCLUDE := -I./include

RELINK := Makefile

# COLORS
ERASE := \033[2K\033[1A\r
CLEAN_TEXT := \033[36m
INFO_TEXT := \033[35m
GOOD_TEXT := \033[32m
BAD_TEXT := \033[31m
RESET := \033[0m


all: $(NAME)
$(NAME): $(DIR_OBJ) $(COMMANDS) $(OBJ)
	echo -e "$(GOOD_TEXT)✅ Making $(NAME) $(BAD_TEXT)"
	$(CC) $(FLAGS) $(OBJ) -o $(NAME) $(COMMANDS) $(INCLUDE)
	printf "$(RESET)"


$(DIR_OBJ):
	mkdir -p $@
$(COMMANDS): FORCE
	+$(MAKE) -sC src/commands

$(DIR_OBJ)/%.o: src/%.cpp $(RELINK)
	echo -e "$(GOOD_TEXT)⏳ Making $(NAME)"
	printf "$(INFO_TEXT) ➥ Making $(RESET)$< $(BAD_TEXT)"
	$(CC) $(FLAGS) -o $@ -c $< $(INCLUDE)
	printf "$(ERASE)"

clean:
	if [[ " $(MAKECMDGOALS)" = *" $@"* ]]; then \
		$(MAKE) -sC src/commands clean; \
		echo -e "$(CLEAN_TEXT)🧹 Cleaning $(NAME)$(RESET)"; \
	fi
	rm -rf $(DIR_OBJ)
fclean: clean
	$(MAKE) -sC src/commands fclean
	echo -e "$(CLEAN_TEXT)🧹 FCleaning $(NAME)$(RESET)"
	rm -f $(NAME)
re: fclean all

FORCE:
.PHONY: all clean fclean re
.SILENT:
-include $(OBJ:.o=.d)
