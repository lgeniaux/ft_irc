#include "Command.hpp"
#include <sstream>

void CommandHandler::handleTOPIC(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    Channel *channel;
    Client  client;
    
    if (tokens.size() < 2) {
        RFC2812Handler::sendResponse(461, server.getClient(client_fd), "TOPIC :Not enough parameters");
        return;
    }
    client = server.getClient(client_fd);
    if (!client.isAuthenticated()) {
        RFC2812Handler::sendResponse(451, server.getClient(client_fd), ":You have not registered");
        return;
    }
    channel = server.getChannel(tokens[1]);
    if (channel == NULL) {
        RFC2812Handler::sendResponse(403, server.getClient(client_fd), tokens[1] + " :No such channel");
        return;
    }
    if (tokens.size() == 2) {
        if (channel->getTopic().empty()) {
            RFC2812Handler::sendResponse(331, server.getClient(client_fd), tokens[1] + " :No topic is set");
            return;
        }
        RFC2812Handler::sendResponse(332, server.getClient(client_fd), tokens[1] + " " + channel->getTopic());
        return;
    }
    if (!channel->hasUser(client.getNickname())) {
        RFC2812Handler::sendResponse(442, server.getClient(client_fd), tokens[1] + " :You're not on that channel");
        return;
    }
    if (!channel->isOperator(client.getNickname())) {
        RFC2812Handler::sendResponse(482, server.getClient(client_fd), tokens[1] + " :You're not channel operator");
        return;
    }
    if (tokens[2] == ":") {
        channel->setTopic("");
        return;
    }
    channel->setTopic(tokens[2]);
    channel->setTopicTime(time(NULL));
    std::string message = ":" + client.getNickname() + " TOPIC " + tokens[1] + " :" + tokens[2] + "\r\n"; 
    channel->broadcastMessageToChannel(message, server, "");
    RFC2812Handler::sendResponse(332, server.getClient(client_fd), tokens[1] + " " + channel->getTopic());
    std::ostringstream oss;
    oss << time(NULL);
    RFC2812Handler::sendResponse(333, server.getClient(client_fd), tokens[1] + " " + client.getNickname() + " " + oss.str());

}
