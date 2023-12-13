#include "Command.hpp"
#include "RFC2812Handler.hpp"

void CommandHandler::handleNICK(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    if (tokens.size() < 2) {
        RFC2812Handler::sendResponse(431, server.clients[client_fd], ":No nickname given");
        return;
    }
    
    if (server.getFdFromNickname(tokens[1]) != -1) {
        RFC2812Handler::sendResponse(433, server.clients[client_fd], tokens[1] + " :Nickname is already in use");
        return;
    }

    std::string newNick = tokens[1];
    std::string oldNick = server.clients[client_fd].getNickname();

    server.clients[client_fd].setNickname(newNick);
    server.clients[client_fd].setNickReceived(true);

    std::map<std::string, Channel>::iterator it;
    for (it = server.channels.begin(); it != server.channels.end(); ++it) {
        if (it->second.isInChannel(oldNick)) {
            it->second.broadcastMessageToChannel(":" + oldNick + " NICK :" + newNick + "\r\n", server, "");
        }
    }
    // Update the nickname to client map in the server
    server.updateNicknameMap(oldNick, newNick, server.clients[client_fd]);
}
