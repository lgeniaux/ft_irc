#include "Command.hpp"

void CommandHandler::handlePING(const std::vector<std::string> &tokens, int client_fd, Server & /*server*/)
{
    (void)tokens;
    send(client_fd, "PONG\r\n", 6, 0);
}