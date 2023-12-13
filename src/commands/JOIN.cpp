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

   if (channel != NULL)
    {
        // Debug Channel mode and invite status
        std::cout << "Channel mode: " << channel->getMode('i') << std::endl;
        std::cout << "Channel invite status: " << channel->isInvited(clientNickname) << std::endl;
        // Debug Channel key and key status
        std::cout << "Channel key: " << channel->checkKey(tokens[2]) << std::endl;
        std::cout << "Channel key status: " << channel->getMode('k') << std::endl;

        //Debug client command arguments
        std::cout << "Client command arguments: " << tokens[1] << " " << tokens[2] << std::endl;
    }

    if (channel == NULL)
    {
        // Channel does not exist, create it
        server.joinChannel(channelName, clientNickname);
        sendJoinSuccessInfo(server, channelName, client_fd);
    }
    else if (channel->getMode('i') && !channel->isInvited(clientNickname))
    {
        rfcHandler.sendResponse(473, server.getClient(client_fd), channelName + " :Cannot join channel (+i)");
    }
    else if (channel->getMode('k') && !channel->checkKey(tokens[2]))
    {
        rfcHandler.sendResponse(475, server.getClient(client_fd), channelName + " :Cannot join channel (+k)");
    }
    else if (channel->getLimit() != 0 && channel->getUsers().size() >= channel->getLimit())
    {
        rfcHandler.sendResponse(471, server.getClient(client_fd), channelName + " :Cannot join channel (+l)");
    }
    else if (!channel->isInChannel(clientNickname))
    {
        server.joinChannel(channelName, clientNickname);
        sendJoinSuccessInfo(server, channelName, client_fd);
        std::cout << "User " << clientNickname << " joined channel " << channelName << std::endl;

        channel = server.getChannel(channelName); // Refresh the channel pointer
        if (channel != NULL && channel->isInvited(clientNickname))
            channel->removeInvite(clientNickname);
    }
    else
    {
        // User is already in the channel
        rfcHandler.sendResponse(443, server.getClient(client_fd), channelName + " :user already on channel");
    }
}