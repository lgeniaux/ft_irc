#include "Command.hpp"
#include <arpa/inet.h>

void CommandHandler::handlePING(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
    (void)tokens;
    Client client = server.getClient(client_fd);
    std::string message = ":localhost PONG\r\n";
    send(client_fd, message.c_str(), message.length(), 0);
    while (!message.empty() &&
           (message[message.size() - 1] == '\r' ||
            message[message.size() - 1] == '\n'))
        message.erase(message.size() - 1);
    std::cout << LIGHT BLUE << "Sending :" << RESET GRAY << message << RESET << std::endl;
}