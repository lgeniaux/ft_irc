#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "RFC2812Handler.hpp"

void CommandHandler::handleJOIN(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    RFC2812Handler rfcHandler;  // Initialize here
    
    if (tokens.size() < 2) {
        rfcHandler.sendResponse(461, server.getClient(client_fd), "JOIN :Not enough parameters");
        return;
    }

    const std::string& channelName = tokens[1];
    Channel* channel = server.getChannel(channelName);

    if (channel == nullptr) {
        server.joinChannel(channelName, client_fd);
    } else if (channel->getMode('i') && !channel->isInvited(client_fd)) {
        rfcHandler.sendResponse(473, server.getClient(client_fd), channelName + " :Cannot join channel (+i)");
        return;
    }

    server.joinChannel(channelName, client_fd);

    std::cout << "User " << client_fd << " joined channel " << channelName << std::endl;

    channel = server.getChannel(channelName);  // Refresh the channel pointer
    if (channel != nullptr && channel->isInvited(client_fd)) {
        channel->removeInvite(client_fd);
    }
}
