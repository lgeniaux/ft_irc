#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "RFC2812Handler.hpp"
#include <sstream>
#include <vector>

void sendJoinSuccessInfo(Server &server, const std::string &channelName, int client_fd)
{
    RFC2812Handler rfcHandler;
    Channel *channel = server.getChannel(channelName);
    if (!channel)
    {
        return;
    }

    // Send the topic of the channel
    if (!channel->getTopic().empty())
    {
        rfcHandler.sendResponse(332, server.getClient(client_fd), channelName + " :" + channel->getTopic());
    }

    // Send the names list
    std::set<std::string> users = channel->getUsers();
    std::string namesList = "";
    std::set<std::string>::const_iterator it;
    for (it = users.begin(); it != users.end(); ++it)
    {
        namesList += *it + " ";
    }
    namesList = namesList.substr(0, namesList.size() - 1);

    rfcHandler.sendResponse(353, server.getClient(client_fd), "= " + channelName + " :" + namesList);
    rfcHandler.sendResponse(366, server.getClient(client_fd), channelName + " :End of NAMES list");
}

void CommandHandler::handleJOIN(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
    RFC2812Handler rfcHandler;

    if (tokens.size() < 2)
    {
        rfcHandler.sendResponse(461, server.getClient(client_fd), "JOIN :Not enough parameters");
        return;
    }

    std::vector<std::string> channels;
    std::vector<std::string> keys;

    // Split channels and keys
    std::istringstream channelStream(tokens[1]);
    std::string channel;
    while (getline(channelStream, channel, ','))
    {
        channels.push_back(channel);
    }

    if (tokens.size() > 2)
    {
        std::istringstream keyStream(tokens[2]);
        std::string key;
        while (getline(keyStream, key, ','))
        {
            keys.push_back(key);
        }
    }

    for (size_t i = 0; i < channels.size(); ++i)
    {
        const std::string &channelName = channels[i];
        std::string key;
        if (i < keys.size())
        {
            key = keys[i];
        }
        else
        {
            key = std::string();
        }

        Channel *channel = server.getChannel(channelName);
        std::string clientNickname = server.getClient(client_fd).getNickname();
        if (clientNickname.empty())
            continue;

        if (channel == NULL)
        {
            server.joinChannel(channelName, clientNickname);
            sendJoinSuccessInfo(server, channelName, client_fd);
        }
        else if (channel->getMode('i') && !channel->isInvited(clientNickname))
        {
            rfcHandler.sendResponse(473, server.getClient(client_fd), channelName + " :Cannot join channel (+i)");
        }
        else if (channel->getMode('k') && !channel->checkKey(key))
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
            channel = server.getChannel(channelName); // Refresh the channel pointer
            if (channel != NULL && channel->isInvited(clientNickname))
                channel->removeInvite(clientNickname);
        }
        else
        {
            rfcHandler.sendResponse(443, server.getClient(client_fd), channelName + " :User already on channel");
        }
    }
}
