#include "Command.hpp"

void CommandHandler::handlePRIVMSG(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames) {
    (void) nicknames;
	if (tokens.size() < 3) {
		std::cerr << "Invalid PRIVMSG command format." << std::endl;
		return;
	}
    std::cout << "Private message from client " << client_fd << " to " << tokens[1] << std::endl;
}
