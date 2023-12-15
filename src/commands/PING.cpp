#include "Command.hpp"
#include <arpa/inet.h>

void CommandHandler::handlePING(const std::vector<std::string> &tokens, int client_fd, Server & server)
{
    (void)tokens;
    Client client = server.getClient(client_fd);
    std::string message = client.getNickname() + "!" + client.getUsername() + "@" + inet_ntoa(client.getAddress().sin_addr) + " PONG\r\n";
    send(client_fd, message.c_str(), message.length(), 0);
}