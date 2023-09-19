#include "Command.hpp"
#include "Server.hpp"
#include "RFC2812Handler.hpp"

void CommandHandler::handlePRIVMSG(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
    RFC2812Handler rfcHandler;
    if (tokens.size() < 3)
    {
        rfcHandler.sendResponse(461, server.getClient(client_fd), "PRIVSMG :Not enough parameters");
        return;
    }
    std::string target = tokens[1];
    std::string message = tokens[2];

    // Debug 
    std::cout << "target: " << target << std::endl;
    std::cout << "message: " << message << std::endl;

    //Craft the message (:Nick!User@Host PRIVMSG #channel/user :message)
    std::string newMessage = ":" + server.getClient(client_fd).getNickname() + " PRIVMSG " + target + " " + message;
    if (target[0] == '#')
    {
        // Channel message
        server.handleChannelMessage(target, newMessage, client_fd);
    }
    else
    {
        // Private message (too lazy to refactor this into a function for the moment)
        if (server.nicknameToClientMap.find(target) != server.nicknameToClientMap.end())
        {
            Client targetClient = server.nicknameToClientMap[target];
            if (targetClient.isAuthenticated())
            {
                int target_fd = targetClient.getFd();
                std::string formattedMessage = rfcHandler.formatMessage(":" + server.getClient(client_fd).getNickname() + " PRIVMSG " + target + " " + message);
                send(target_fd, formattedMessage.c_str(), formattedMessage.size(), 0);
            }
            else
                RFC2812Handler::sendResponse(401, server.getClient(client_fd), target + " :No such nick");
        }
        else
            RFC2812Handler::sendResponse(401, server.getClient(client_fd), target + " :No such nick");
    }
}
