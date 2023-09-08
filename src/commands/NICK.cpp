#include "Command.hpp"
#include "RFC2812Handler.hpp"

void CommandHandler::handleNICK(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    if (tokens.size() < 2) {
        RFC2812Handler::sendResponse(431, server.clients[client_fd], ":No nickname given");
        return;
    }

    std::string nick = tokens[1];
    // MORE TO ADD HERE (duplicates nick, etc.)
    
    server.clients[client_fd].setNickname(nick);
    server.clients[client_fd].setNickReceived(true);
}
