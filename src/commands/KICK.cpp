#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"
#include <unistd.h>

void PART(std::string nickname, std::string channel, std::string message, Server& server);

void CommandHandler::handleKICK(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    if (tokens.size() < 2) {
        RFC2812Handler::sendResponse(461, server.clients[client_fd], "KICK :Not enough parameters");
        return;
    }
    if (!server.getClient(client_fd).isAuthenticated()) {
        RFC2812Handler::sendResponse(451, server.clients[client_fd], ":You have not registered");
        return;
    }
    std::string message = ": User " + tokens[2] + " has been kicked";
    if (tokens.size() > 3 && tokens[3] != ":")
        message = " " + tokens[3];
    PART(tokens[2], tokens[1], message, server);
}
