#include "Command.hpp"

void CommandHandler::handleNICK(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    if (tokens.size() < 2) {
        std::string response = "Not enough arguments for NICK.\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }
    std::string nick = tokens[1];
    server.clients[client_fd].setNickname(nick);
    server.clients[client_fd].setNickReceived(true);
    std::string response = "Nickname set to " + nick + "\n";
    send(client_fd, response.c_str(), response.length(), 0);
}
