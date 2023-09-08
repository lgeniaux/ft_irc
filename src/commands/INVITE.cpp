#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"


void CommandHandler::handleINVITE(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    if (tokens.size() < 3) {
        std::string response = "Not enough arguments";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    int target_fd = std::stoi(tokens[1]);
    const std::string& channelName = tokens[2];
    Channel* channel = server.getChannel(channelName);

    if (!channel->isOperator(client_fd)) {
        std::string response = "You must be an operator to invite users to this channel\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    channel->inviteUser(target_fd);
    // For operator
    std::string response = "You have invited " + std::to_string(target_fd) + " to " + channelName + "\n";
    send(client_fd, response.c_str(), response.length(), 0);

    //For target
    response = "You have been invited to " + channelName + " by " + std::to_string(client_fd) + "\n" ;
    send(target_fd, response.c_str(), response.length(), 0);
}
