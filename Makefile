NAME = ircserv
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I./include
SRC = src/main.cpp src/Server.cpp src/Client.cpp src/Channel.cpp
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(NAME) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
