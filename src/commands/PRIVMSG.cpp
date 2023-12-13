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
    if (target[0] == '#') {
        if (server.getChannel(target) == NULL) {
            rfcHandler.sendResponse(403, server.getClient(client_fd), target + " :No such channel");
            return;
        }
        else if (!server.getChannel(target)->isInChannel(server.getClient(client_fd).getNickname())) {
            rfcHandler.sendResponse(442, server.getClient(client_fd), target + " :You're not on that channel");
            return;
        }
    }

    // The message is everything after the target
    std::string message = "";
    for (std::vector<std::string>::size_type i = 2; i < tokens.size(); i++)
    {
        message += tokens[i] + " ";
    }
    // Remove the trailing space
    if (message.size() > 0)
        message.pop_back();

    // Debug
    // std::cout << "target: "
    //           << "\"" << target << "\"" << std::endl;
    // std::cout << "message: " << message << std::endl;

    // Get the sender's nickname
    std::string sender_nick = server.getClient(client_fd).getNickname();
    // Craft the message (:Nick!User@Host PRIVMSG #channel/user :message) User and Host seem to be optional
    std::string newMessage = rfcHandler.formatMessage(":" + server.getClient(client_fd).getNickname() + " PRIVMSG " + target + " " + message);
    if (target[0] == '#')
    {
        //check if user is in channel
        if (!server.getChannel(target)->isInChannel(sender_nick))
        {
            RFC2812Handler::sendResponse(442, server.getClient(client_fd), target + " :You're not on that channel");
            return;
        }
        // Channel message
        server.handleChannelMessage(target, newMessage, sender_nick);
    }
    else
    {
        // Private message (too lazy to refactor this into a function for the moment)
        if (server.nicknameToClientMap.find(target) != server.nicknameToClientMap.end())
        {
            // std::cout << "Target found: " << target << std::endl;
            Client *targetClient = server.nicknameToClientMap[target];
            // DEBUG list all authenticated clients
            // std::cout << "Authenticated clients: " << std::endl;
            // for (std::map<int, Client>::iterator it = server.clients.begin(); it != server.clients.end(); ++it)
            // {
            //     if (it->second.isAuthenticated())
            //     {
            //         std::cout << "\"" << it->second.getNickname() << "\"" << std::endl;
            //     }
            // }

            // std::cout << "Target Client details: ";
            // std::cout << "Nickname: " << targetClient->getNickname();
            // std::cout << ", FD: " << targetClient->getFd();
            // std::cout << ", Authenticated: " << (targetClient->isAuthenticated() ? "Yes" : "No") << std::endl;

            if (targetClient->isAuthenticated())
            {
                // std::cout << "Target is authenticated" << std::endl;
                int target_fd = targetClient->getFd();
                std::string formattedMessage = rfcHandler.formatMessage(":" + server.getClient(client_fd).getNickname() + " PRIVMSG " + target + " " + message);
                send(target_fd, formattedMessage.c_str(), formattedMessage.size(), 0);
            }
            else
            {
                // std::cout << "Target is not authenticated" << std::endl;
                RFC2812Handler::sendResponse(401, server.getClient(client_fd), target + " :No such nick");
            }
        }
        else
        {
            // std::cout << "Target not found: "
            //           << "\"" << target << "\"" << std::endl;
            RFC2812Handler::sendResponse(401, server.getClient(client_fd), target + " :No such nick");
        }
    }
}
