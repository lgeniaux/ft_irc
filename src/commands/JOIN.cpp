#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"


void CommandHandler::handleJOIN(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    if (tokens.size() < 2) {
        std::string response = "Not enough arguments";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    const std::string& channelName = tokens[1];
    Channel& channel = server.getChannel(channelName);

    if (channel.getMode('i') && !channel.isInvited(client_fd)) {
        std::string response = "You must be invited to join this channel";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }
    

    server.joinChannel(channelName, client_fd);
    std::string response = "You have joined " + channelName + "\n";
    send(client_fd, response.c_str(), response.length(), 0);
    //if he had an invite, remove it
    if (channel.isInvited(client_fd)) 
        channel.removeInvite(client_fd);

}
