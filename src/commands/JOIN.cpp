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

    // Obtain the client's nickname based on their fd using the server's nickname to client map
    std::string clientNickname = server.getClient(client_fd).getNickname();
    if (clientNickname.empty()) {
        // Handle the error, e.g., by sending a message to the client
        return;
    }

    if (channel != NULL){
        if (channel->getMode('i') && !channel->isInvited(clientNickname)) {
            rfcHandler.sendResponse(473, server.getClient(client_fd), channelName + " :Cannot join channel (+i)");
            return;
        }
        else if (channel->hasUser(clientNickname)) {
            rfcHandler.sendResponse(443, server.getClient(client_fd), channelName + " :is already on channel");
            return;
        }
    }

    server.joinChannel(channelName, clientNickname);

    std::cout << clientNickname << " joined channel " << channelName << std::endl;

    channel = server.getChannel(channelName);  // Refresh the channel pointer
    if (channel != NULL && channel->isInvited(clientNickname)) {
        channel->removeInvite(clientNickname);
    }
}

