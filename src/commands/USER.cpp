#include "Command.hpp"
#include "RFC2812Handler.hpp"

void CommandHandler::handleUSER(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
    if (server.clients[client_fd].isAuthenticated())
    {
        RFC2812Handler::sendResponse(462, server.clients[client_fd], ":Unauthorized command (already registered)");
        return;
    }

    if (tokens.size() < 5)
    {
        RFC2812Handler::sendResponse(461, server.clients[client_fd], "USER :Not enough parameters");
        return;
    }

    std::string username = tokens[1];
    std::string realname = tokens[4];

    server.clients[client_fd].setUsername(username);
    server.clients[client_fd].setRealname(realname);
    server.clients[client_fd].setUserReceived(true);
}
