#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include <sstream>
#include <iterator>
#include <map>
#include <unistd.h>

void CommandHandler::handleCommand(const std::string &command, int client_fd, Server &server, bool login)
{

    std::istringstream iss(command);
    std::istream_iterator<std::string> issIt(iss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> tokens;

    std::map<std::string, void (*)(const std::vector<std::string> &, int, Server &)> commandRegistry;

    commandRegistry["JOIN"] = handleJOIN;
    commandRegistry["KICK"] = handleKICK;
    commandRegistry["INVITE"] = handleINVITE;
    commandRegistry["TOPIC"] = handleTOPIC;
    commandRegistry["MODE"] = handleMODE;
    commandRegistry["NICK"] = handleNICK;
    commandRegistry["PASS"] = handlePASS;
    commandRegistry["USER"] = handleUSER;
    commandRegistry["CAP"] = handleCAP;
    commandRegistry["PRIVMSG"] = handlePRIVMSG;
    commandRegistry["PART"] = handlePART;
    commandRegistry["QUIT"] = handleQUIT;
    commandRegistry["PING"] = handlePING;

    for (; issIt != end; ++issIt)
    {
        tokens.push_back(*issIt);
    }

    if (tokens.empty())
    {
        return;
    }

    std::string cmd = tokens[0];
    std::cout << LIGHT GREEN << "cmd: " << RESET BOLD << cmd << RESET GRAY;
    for (size_t i = 1; i < tokens.size(); ++i)
        std::cout << " " << tokens[i];
    std::cout << RESET << std::endl;
    if (commandRegistry.find(cmd) != commandRegistry.end())
    {   
        if (login)
        {
            if (tokens[0] != "USER" && tokens[0] != "NICK" && tokens[0] != "PASS" && tokens[0] != "CAP")
                return;
        }
        commandRegistry[cmd](tokens, client_fd, server);
        return;
    }
    std::cerr << WARN << "Unknown command: " << cmd << std::endl;
    return;
}
