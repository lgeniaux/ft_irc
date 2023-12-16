#include "Command.hpp"

void CommandHandler::handleCAP(const std::vector<std::string> &tokens, int client_fd, Server & /*server*/)
{
    if (tokens.size() >= 2 && tokens[1] == "LS")
    {
        std::string response = ":server CAP * LS :\r\n";
        send(client_fd, response.c_str(), response.length(), 0);
    }
}
