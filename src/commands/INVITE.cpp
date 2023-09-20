#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "RFC2812Handler.hpp"

void CommandHandler::handleINVITE(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    RFC2812Handler rfcHandler;

    if (tokens.size() < 3) {
        rfcHandler.sendResponse(461, server.getClient(client_fd), "INVITE :Not enough parameters");
        return;
    }

    const std::string& recipientNickname = tokens[1];
    const std::string& channelName = tokens[2];

    //get recipient fd
    int recipient_fd = server.getFdFromNickname(recipientNickname);
    if (recipient_fd == -1) {
        rfcHandler.sendResponse(401, server.getClient(client_fd), recipientNickname + " :No such nick");
        return;
    }

    Channel* channel = server.getChannel(channelName);
    if (channel == NULL) {
        rfcHandler.sendResponse(403, server.getClient(client_fd), channelName + " :No such channel");
        return;
    }
    //if user is already in channel (Channel class has isInChannel method)
    if (channel->isInChannel(recipientNickname)) {
        rfcHandler.sendResponse(443, server.getClient(client_fd), recipientNickname + " " + channelName + " :is already on channel");
        return;
    }
    //if the sending client is not in the channel
    if (!channel->isInChannel(server.getClient(client_fd).getNickname())) {
        rfcHandler.sendResponse(442, server.getClient(client_fd), channelName + " :You're not on that channel");
        return;
    }
    //if the channel is invite only and the sending client is not an operator
    if (channel->getMode('i') && !channel->isOperator(server.getClient(client_fd).getNickname())) {
        rfcHandler.sendResponse(482, server.getClient(client_fd), channelName + " :You're not channel operator");
        return;
    }
    //if the recipient is already invited or in the channel
    if (channel->isInvited(recipientNickname)) {
        rfcHandler.sendResponse(443, server.getClient(client_fd), recipientNickname + " " + channelName + " :is already invited / is already on channel");
        return;
    }

    //if no error conditions are met, update the status and send the invite (:NICK!USER@HOST INVITE recipient channel)
    channel->inviteUser(recipientNickname);
    std::string formattedMessage = ":" + server.getClient(client_fd).getNickname() + " INVITE " + recipientNickname + " " + channelName;
    send(recipient_fd, formattedMessage.c_str(), formattedMessage.length(), 0);
}
