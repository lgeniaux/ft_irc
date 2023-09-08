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
    Channel* channel = server.getChannel(channelName);
    if (channel == NULL) { //if the channel does not exist getChannel returns  "return *(new Channel("null", "null"));""
        //a bit tricky but the channel is created here and the user automaticly joins it
        server.joinChannel(channelName, client_fd);
    }
    else if (channel->getMode('i') && !channel->isInvited(client_fd)) { // here we check if the channel is invite only and if the user is invited
        std::string response = "You must be invited to join this channel";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }
    
    //if the channel exists and is not in invite only mode or if the user is invited we join the channel
    server.joinChannel(channelName, client_fd);
    //Complying RFC 2812 response (incomplete for the moment)
    // std::string response = ":" + server.getHostname() + " 332 " + std::to_string(client_fd) + " " + channelName + " :" + channel.getTopic() + "\n";
    std::cout << "User " << client_fd << " joined channel " << channelName << std::endl;
    //if he had an invite, remove it
    if (channel->isInvited(client_fd)) 
        channel->removeInvite(client_fd);

}
