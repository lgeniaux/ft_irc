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
    else if (tokens.size() > 3)
    {
        if (client.isNickReceived() == 2)
        {
            client.setNickname(client.getNickname() + "_");
            client.setNickReceived(RECEIVED);
        }
        return;
    }

    if (server.getFdFromNickname(tokens[1]) != -1)
    {
        RFC2812Handler::sendResponse(433, client, tokens[1] + " :Nickname is already in use");
        if (client.getNickname() == "")
        {
            client.setNickname(tokens[1]);
            client.setNickReceived(CONFLICT);
        }
        return;
    }

    std::string newNick = tokens[1];
    std::string oldNick = client.getNickname();

    client.setNickname(newNick);
    client.setNickReceived(RECEIVED);

    std::map<std::string, Channel>::iterator it;
    for (it = server.channels.begin(); it != server.channels.end(); ++it)
    {
        if (it->second.isInChannel(oldNick))
        {
            it->second.broadcastMessageToChannel(":" + oldNick + " NICK :" + newNick + "\r\n", server, "");
        }
    }
    // Update the nickname to client map in the server
    server.updateNicknameMap(oldNick, newNick, client);
    std::cout << client_fd << "Nick received: " << static_cast<int>(client.isNickReceived()) << std::endl;
}
