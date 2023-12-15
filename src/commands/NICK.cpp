#include "Command.hpp"
#include "RFC2812Handler.hpp"

void CommandHandler::handleNICK(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
    Client& client = server.clients[client_fd];

    if (tokens.size() < 2)
    {
        RFC2812Handler::sendResponse(431, client, ":No nickname given");
        return;
    }
    // Check if the nickname is in conflict and it should be set by the server
    else if (tokens.size() > 2 && client.getNickReceived() == CONFLICT)
    {
        while (server.getFdFromNickname(client.getNickname()) != -1)
        {
            client.setNickname(client.getNickname() + "_");
            if (client.getNickname().size() > 9)
            {
                RFC2812Handler::sendResponse(432, client, ":Erroneous nickname");
                server.markClientForDisconnection(client_fd);
                return;
            }
        }
        client.setNickReceived(RECEIVED);
        server.updateNicknameMap("", client.getNickname(), client);
        return;
    }

    std::string newNick = tokens[1];
    std::string oldNick = client.getNickname();

    // Check if the nickname is not already in use
    if (server.getFdFromNickname(tokens[1]) != -1)
    {
        RFC2812Handler::sendResponse(433, client, tokens[1] + " :Nickname is already in use");
        if (oldNick == "")
        {
            client.setNickname(tokens[1]);
            client.setNickReceived(CONFLICT);
        }
        return;
    }

    // Check if the nickname is valid
    if (newNick.size() > 9
        || newNick.find_first_of("0123456789-") == 0
        || newNick.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-[]\\`^{}|") != std::string::npos)
    {
        RFC2812Handler::sendResponse(432, client, ":Erroneous nickname");
        return;
    }

    client.setNickname(newNick);
    client.setNickReceived(RECEIVED);

    std::map<std::string, Channel>::iterator it;
    std::string message = ":" + oldNick + " NICK :" + newNick + "\r\n";
    bool isBroadcasted = false;
    std::set<std::string> commonUsers = server.getCommonUsers(oldNick);

    // Broadcast the message to all user that shares a channel with the client
    for (std::set<std::string>::iterator it = commonUsers.begin(); it != commonUsers.end(); ++it)
    {
        std::string nickname = *it;
        int fd = server.getFdFromNickname(nickname);
        if (fd != -1)
        { // Check if fd is valid
            send(fd, message.c_str(), message.length(), 0);
            isBroadcasted = true;
        }
    }
    
    if (client.isAuthenticated() && !isBroadcasted)
    {
        if (client_fd != -1)
        { // Check if fd is valid
            send(client_fd, message.c_str(), message.length(), 0);
        }
    }
    // Update the nickname to client map in the server
    server.updateNicknameMap(oldNick, newNick, client);
}
