#include "Command.hpp"
#include <unistd.h>

void CommandHandler::handlePASS(const std::vector<std::string>& tokens, int client_fd, Server& server) {
    // Check if the client is already authenticated
    if (server.clients[client_fd].isAuthenticated()) {
        std::string response = "Already authenticated. PASS command is invalid now.\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    if (tokens.size() < 2) {
        std::string response = "Not enough arguments for PASS.\n";
        send(client_fd, response.c_str(), response.length(), 0);
        return;
    }

    std::string suppliedPassword = tokens[1];

    // Check against the server's password
    if (suppliedPassword != server.password) {
        std::string auth_failed = "Authentication failed.\n";
        send(client_fd, auth_failed.c_str(), auth_failed.size(), 0);
        close(client_fd);
        server.clients.erase(client_fd);
        return;
    }

    // Set the client as authenticated
    server.clients[client_fd].setPassReceived(true);
}
