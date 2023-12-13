#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"
#include <unistd.h>

void CommandHandler::handleKICK(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    std::pair<Channel*, Client> preCheck;
    Channel *channel;
    Client  client;

    if (tokens.size() < 3) {
        RFC2812Handler::sendResponse(461, server.clients[client_fd], "KICK :Not enough parameters");
        return;
    }
    preCheck = preChecks(tokens[1], client_fd, server, true);
    channel = preCheck.first;
    client = preCheck.second;
    if (channel == NULL)
        return;
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
