#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include <algorithm>
#include <sstream>
#include <arpa/inet.h>

void CommandHandler::handleMODE(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
    std::string message;
    RFC2812Handler rfcHandler;
    Channel *channel;
    Client &client = server.getClient(client_fd);
    std::string nickname = client.getNickname();
    std::string nicknameBroadcast = nickname + "!" + client.getUsername() + "@" + inet_ntoa(client.getAddress().sin_addr);

    if (tokens.size() == 2)
    {
        std::string channelName = tokens[1];
        if (server.getChannel(channelName) == NULL)
        {
            rfcHandler.sendResponse(403, client, channelName + " :No such channel");
            return;
        }
        else
        {
            channel = server.getChannel(channelName);
            if (!channel->isInChannel(nickname))
            {
                rfcHandler.sendResponse(442, client, channelName + " :You're not on that channel");
                return;
            }
            else
            {
                std::ostringstream oss;
                oss << channel->getLimit();
                std::string limitStr = oss.str();
                if (limitStr == "0")
                    limitStr = "";
                message = channelName + " " + channel->getModes() + " " + channel->getKey() + " " + limitStr;
                rfcHandler.sendResponse(324, client, message);
                return;
            }
        }
    }

    channel = preChecks(tokens[1], client_fd, server, true);
    if (channel == NULL)
        return;

    const std::string channelName = tokens[1];
    const std::string &mode = tokens[2];

    // Differentiate between - and + modes (e.g: MODE #chan +o nick)
    const char modeSign = mode[0];

    // list of supported modes (ugly because of c++98)
    std::vector<char> supportedModes;
    supportedModes.push_back('i');
    supportedModes.push_back('t');
    supportedModes.push_back('k');
    supportedModes.push_back('o');
    supportedModes.push_back('l');

    // Irssi sends '+' by default (if channelname contains # on input), so the check is not necessary but I'll leave it here
    if (modeSign != '+' && modeSign != '-')
    {
        rfcHandler.sendResponse(461, client, "MODE :Invalid mode");
        return;
    }

    if (mode.size() > 2)
    {
        rfcHandler.sendResponse(501, client, "MODE :Unknown MODE flag");
        return;
    }

    // Check if the mode is supported
    if (std::find(supportedModes.begin(), supportedModes.end(), mode[1]) == supportedModes.end())
    {
        rfcHandler.sendResponse(501, client, "MODE :Unknown MODE flag");
        return;
    }

    // Check if the mode is valid
    if (mode[1] == 'k')
    {
        if (modeSign == '+')
        {
            if (tokens.size() < 4)
            {
                rfcHandler.sendResponse(461, client, "MODE :Not enough parameters");
                return;
            }
            channel->setKey(tokens[3]);
            channel->setMode(mode[1], true);
            message = channelName + " +" + mode[1] + " " + tokens[3];
            rfcHandler.sendResponse(324, client, message);
            channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + message + "\r\n", server, nickname);
        }
        else if (modeSign == '-')
        {
            std::cout << channel->getMode(mode[1]) << std::endl;
            if (channel->getMode(mode[1]) == false)
            {
                rfcHandler.sendResponse(501, client, "MODE :flag is already set or unset");
                return;
            }
            channel->setKey("");
            channel->setMode(mode[1], false);
            message = channelName + " -" + mode[1];
            rfcHandler.sendResponse(324, client, message);
            channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + message + "\r\n", server, nickname);
        }
    }
    else if (mode[1] == 'l')
    {
        if (modeSign == '+')
        {
            if (tokens.size() < 4)
            {
                rfcHandler.sendResponse(461, client, "MODE :Not enough parameters");
                return;
            }
            channel->setLimit(atoi(tokens[3].c_str()));
            channel->setMode(mode[1], true);
            message = channelName + " +" + mode[1] + " " + tokens[3];
            rfcHandler.sendResponse(324, client, message);
            channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + message + "\r\n", server, nickname);
        }
        else if (modeSign == '-')
        {
            if (channel->getMode(mode[1]) == false)
            {
                rfcHandler.sendResponse(501, client, "MODE :flag is already set or unset");
                return;
            }
            channel->setLimit(0);
            channel->setMode(mode[1], false);
            message = channelName + " -" + mode[1];
            rfcHandler.sendResponse(324, client, channelName + " -" + mode[1]);
            channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + channelName + " -" + mode[1], server, nickname);
        }
    }
    else if (mode[1] == 't')
    {
        if (modeSign == '+')
        {
            if (channel->getMode(mode[1]) == true)
            {
                rfcHandler.sendResponse(501, client, "MODE :flag is already set or unset");
                return;
            }
            channel->setMode(mode[1], true);
            message = channelName + " +" + mode[1];
            rfcHandler.sendResponse(324, client, message);
            channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + message + "\r\n", server, nickname);
        }
        else if (modeSign == '-')
        {
            if (channel->getMode(mode[1]) == false)
            {
                rfcHandler.sendResponse(501, client, "MODE :flag is already set or unset");
                return;
            }
            channel->setMode(mode[1], false);
            message = channelName + " -" + mode[1];
            rfcHandler.sendResponse(324, client, message);
            channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + message + "\r\n", server, nickname);
        }
    }
    else if (mode[1] == 'o')
    {
        if (tokens.size() < 4)
        {
            rfcHandler.sendResponse(461, client, "MODE :Not enough parameters");
            return;
        }
        if (modeSign == '+')
            channel->addOperator(tokens[3]);
        else
            channel->removeOperator(tokens[3]);
        message = channelName + " " + modeSign + mode[1] + " " + tokens[3];
        rfcHandler.sendResponse(324, client, message);
        channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + message + "\r\n", server, nickname);
    }
    else if (mode[1] == 'i')
    {
        if (modeSign == '+')
        {
            if (channel->getMode(mode[1]) == true)
            {
                rfcHandler.sendResponse(501, client, "MODE :flag is already set or unset");
                return;
            }
            channel->setMode(mode[1], true);
            message = channelName + " +" + mode[1];
            rfcHandler.sendResponse(324, client, message);
            channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + message + "\r\n", server, nickname);
        }
        else if (modeSign == '-')
        {
            if (channel->getMode(mode[1]) == false)
            {
                rfcHandler.sendResponse(501, client, "MODE :flag is already set or unset");
                return;
            }
            channel->setMode(mode[1], false);
            message = channelName + " -" + mode[1];
            rfcHandler.sendResponse(324, client, message);
            channel->broadcastMessageToChannel(":" + nicknameBroadcast + " MODE " + message + "\r\n", server, nickname);
        }
    }
}
