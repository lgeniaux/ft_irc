#include "Command.hpp"

void CommandHandler::handleUSER(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    // Check if the client is already authenticated
    if (server.clients[client_fd].isAuthenticated()) {
        std::string response = "Already authenticated. USER command is invalid now.\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    // Check if enough arguments are present
    if (tokens.size() < 5) {
        std::string response = "Not enough arguments for USER.\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    std::string username = tokens[1];
    std::string realname = tokens[4];

    server.clients[client_fd].setUsername(username);
    server.clients[client_fd].setRealname(realname);
    server.clients[client_fd].setUserReceived(true);
    
    std::string response = "Username and real name set.\n";
    send(client_fd, response.c_str(), response.length(), 0);
}
