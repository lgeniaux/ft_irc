#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include <algorithm>

void CommandHandler::handleMODE(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
    RFC2812Handler rfcHandler;
    std::pair<Channel *, Client> preChecksResult;
    Channel *channel;
    Client client;

    if (tokens.size() == 2)
    {
        std::string channelName = tokens[1];
        if (server.getChannel(channelName) == NULL)
        {
            rfcHandler.sendResponse(403, server.getClient(client_fd), channelName + " :No such channel");
            return;
        }
        else
        {
            channel = server.getChannel(channelName);
            if (!channel->isInChannel(server.getClient(client_fd).getNickname()))
            {
                rfcHandler.sendResponse(442, server.getClient(client_fd), channelName + " :You're not on that channel");
                return;
            }
            else
            {
                rfcHandler.sendResponse(324, server.getClient(client_fd), channelName + " +" + channel->getModes());
                return;
            }
        }
    }

    preChecksResult = preChecks(tokens[1], client_fd, server, true);
    channel = preChecksResult.first;
    client = preChecksResult.second;
    if (channel == NULL)
        return;

    const std::string channelName = tokens[1];
    const std::string &mode = tokens[2];
    // Arguments will be checked later in the code

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
        rfcHandler.sendResponse(461, server.getClient(client_fd), "MODE :Invalid mode");
        return;
    }

    // For the moment we do not support multiple modes at once (isn't ask in the subject either)
    if (mode.size() > 2)
    {
        rfcHandler.sendResponse(501, server.getClient(client_fd), "MODE :Unknown MODE flag");
        return;
    }

    // Check if the mode is supported
    if (std::find(supportedModes.begin(), supportedModes.end(), mode[1]) == supportedModes.end())
    {
        rfcHandler.sendResponse(501, server.getClient(client_fd), "MODE :Unknown MODE flag");
        return;
    }

    // Check if the mode is valid
    if (mode[1] == 'k')
    {
        if (modeSign == '+')
        {
            if (tokens.size() < 4)
            {
                rfcHandler.sendResponse(461, server.getClient(client_fd), "MODE :Not enough parameters");
                return;
            }
            channel->setKey(tokens[3]);
            channel->setMode(mode[1], true);
            rfcHandler.sendResponse(324, server.getClient(client_fd), channelName + " +" + mode[1] + " " + tokens[3]);
        }
        else if (modeSign == '-')
        {
            std::cout << channel->getMode(mode[1]) << std::endl;
            if (channel->getMode(mode[1]) == false)
            {
                rfcHandler.sendResponse(501, server.getClient(client_fd), "MODE :flag is already set or unset");
                return;
            }
            channel->setKey("");
            channel->setMode(mode[1], false);
            rfcHandler.sendResponse(324, server.getClient(client_fd), channelName + " -" + mode[1]);
        }
    }
    else if (mode[1] == 'l')
    {
        if (modeSign == '+')
        {
            if (tokens.size() < 4)
            {
                rfcHandler.sendResponse(461, server.getClient(client_fd), "MODE :Not enough parameters");
                return;
            }
            channel->setLimit(atoi(tokens[3].c_str()));
            channel->setMode(mode[1], true);
            rfcHandler.sendResponse(324, server.getClient(client_fd), channelName + " +" + mode[1] + " " + tokens[3]);
        }
        else if (modeSign == '-')
        {
            if (channel->getMode(mode[1]) == false)
            {
                rfcHandler.sendResponse(501, server.getClient(client_fd), "MODE :flag is already set or unset");
                return;
            }
            channel->setLimit(0);
            channel->setMode(mode[1], false);
            rfcHandler.sendResponse(324, server.getClient(client_fd), channelName + " -" + mode[1]);
        }
    }
    else if (mode[1] == 't')
    {
        if (modeSign == '+')
        {
            if (channel->getMode(mode[1]) == true)
            {
                rfcHandler.sendResponse(501, server.getClient(client_fd), "MODE :flag is already set or unset");
                return;
            }
            channel->setMode(mode[1], true);
            rfcHandler.sendResponse(324, server.getClient(client_fd), channelName + " +" + mode[1]);
        }
        else if (modeSign == '-')
        {
            if (channel->getMode(mode[1]) == false)
            {
                rfcHandler.sendResponse(501, server.getClient(client_fd), "MODE :flag is already set or unset");
                return;
            }
            channel->setMode(mode[1], false);
            rfcHandler.sendResponse(324, server.getClient(client_fd), channelName + " -" + mode[1]);
        }
    }
    else if (mode[1] == 'o')
    {
        if (tokens.size() < 4)
        {
            rfcHandler.sendResponse(461, server.getClient(client_fd), "MODE :Not enough parameters");
            return;
        }
        if (modeSign == '+')
            channel->addOperator(tokens[3]);
        else
            channel->removeOperator(tokens[3]);
    }
    else if (mode[1] == 'i' || mode[1] == 't')
    {
        channel->setMode(mode[1], modeSign == '+');
        rfcHandler.sendResponse(324, server.getClient(client_fd), channelName + modeSign + mode[1]);
    }
}
