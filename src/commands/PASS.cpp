#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include <unistd.h>

void CommandHandler::handlePASS(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
    Client &client = server.clients[client_fd];
    if (client.isAuthenticated())
    {
        RFC2812Handler::sendResponse(462, client, ":You may not reregister");
        return;
    }

    if (tokens.size() < 2)
    {
        RFC2812Handler::sendResponse(461, client, "PASS :Not enough parameters");
        return;
    }

    std::string suppliedPassword = tokens[1];
    std::cout << suppliedPassword << std::endl;
    if (suppliedPassword != server.password)
    {
        RFC2812Handler::sendResponse(464, client, ":Password incorrect");
        client.setPassReceived(WRONG);
        client.setClientQuit(true);
        server.markClientForDisconnection(client_fd);
        return;
    }
    else
    {
        client.setPassReceived(RECEIVED);
        // Authentication check
        if (client.isPassReceived() == RECEIVED && client.getNickReceived() == RECEIVED && client.isUserReceived() && !client.isAuthenticated())
        {       
            client.setAuthenticated(true);
            RFC2812Handler::sendInitialConnectionMessages(client);
        }
    }
}
