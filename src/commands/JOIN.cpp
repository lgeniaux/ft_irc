#include "Command.hpp"

void CommandHandler::handleJOIN(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames) {
	(void)nicknames;
	if (tokens.size() != 2) {
    	std::cerr << "Invalid JOIN command format." << std::endl;
        return;
    }
    std::cout << "Client " << client_fd << " wants to join channel " << tokens[1] << std::endl;
}