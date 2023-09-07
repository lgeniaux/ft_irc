#include "Command.hpp"

void CommandHandler::handleNICK(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames) {
	if (tokens.size() != 2) {
        std::cerr << "Invalid NICK command format." << std::endl;
        return;
    }
    std::string nickname = tokens[1];
    nicknames[client_fd] = nickname;
    std::cout << "Nickname set to " << nickname << " for client " << client_fd << std::endl;
}