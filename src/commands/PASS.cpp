#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include <unistd.h>


void CommandHandler::handlePASS(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    if (server.clients[client_fd].isAuthenticated()) {
        RFC2812Handler::sendResponse(462, server.clients[client_fd], ":You may not reregister");
        return;
    }

    if (tokens.size() < 2) {
        RFC2812Handler::sendResponse(461, server.clients[client_fd], "PASS :Not enough parameters");
        return;
    }

    std::string suppliedPassword = tokens[1];
    if (suppliedPassword != server.password) {
        RFC2812Handler::sendResponse(464, server.clients[client_fd], ":Password incorrect");
        server.markClientForDisconnection(client_fd);
        return;
    }
    else{
        server.clients[client_fd].setPassReceived(true);
    }
}
