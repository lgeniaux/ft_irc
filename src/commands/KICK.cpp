#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"
#include <unistd.h>

void PART(std::string nickname, std::string channel, std::string message, Server& server);

void CommandHandler::handleKICK(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    Channel *channel;
    Client  client;

    if (tokens.size() < 3) {
        RFC2812Handler::sendResponse(461, server.clients[client_fd], "KICK :Not enough parameters");
        return;
    }
    client = server.getClient(client_fd);
    if (!client.isAuthenticated()) {
        RFC2812Handler::sendResponse(451, server.clients[client_fd], ":You have not registered");
        return;
    }
    channel = server.getChannel(tokens[1]);
    if (channel == NULL) {
        RFC2812Handler::sendResponse(403, server.clients[client_fd], tokens[1] + " :No such channel");
        return;
    }
    if (!channel->isOperator(client.getNickname())) {
        RFC2812Handler::sendResponse(482, server.clients[client_fd], tokens[1] + " :You're not channel operator");
        return;
    }
    std::string message = ": User " + tokens[2] + " has been kicked";
    if (tokens.size() > 3 && tokens[3] != ":")
        message = " " + tokens[3];
    PART(tokens[2], tokens[1], message, server);
}
