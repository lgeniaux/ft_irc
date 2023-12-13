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
    if (!channel->isInChannel(tokens[2])) {
        RFC2812Handler::sendResponse(441, server.clients[client_fd], tokens[2] + " " + tokens[1] + " :They aren't on that channel");
        return;
    }
    if (server.getFdFromNickname(tokens[2]) == -1) {
        RFC2812Handler::sendResponse(401, server.clients[client_fd], tokens[2] + " :No such nick/channel");
        return;
    }
    std::cout << tokens[1] << std::endl;
    std::string message = ":" + client.getNickname() + " KICK " + tokens[1] + " " + tokens[2] + " ";
    if (tokens.size() > 3 && tokens[3] != ":")
        message += tokens[3];
    else
        message += ":" + client.getNickname();
    channel->broadcastMessageToChannel(message + "\r\n", server, "");
	server.leaveChannel(channel->getName(), tokens[2]);
}
