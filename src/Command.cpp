#include "Command.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include <sstream>
#include <iterator>
#include <tr1/unordered_map>
#include <unistd.h>

void CommandHandler::handleCommand(const std::string& command, int client_fd, Server& server) {
    std::istringstream iss(command);
    std::istream_iterator<std::string> issIt(iss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> tokens;

    std::tr1::unordered_map<std::string, void (*)(const std::vector<std::string>&, int, Server&)> commandRegistry;


    commandRegistry["JOIN"] = handleJOIN;
    commandRegistry["KICK"] = handleKICK;
    commandRegistry["INVITE"] = handleINVITE;
    commandRegistry["TOPIC"] = handleTOPIC;
    commandRegistry["MODE"] = handleMODE;
    commandRegistry["NICK"] = handleNICK;
    commandRegistry["PASS"] = handlePASS;
    commandRegistry["USER"] = handleUSER;


    for (; issIt != end; ++issIt) {
        tokens.push_back(*issIt);
    }
    
    if (tokens.empty()) {
        return;
    }

    std::string cmd = tokens[0];

    if (commandRegistry.find(cmd) != commandRegistry.end()) {
        commandRegistry[cmd](tokens, client_fd, server);
        return;
    }
    std::cerr << "Unknown command: " << cmd << std::endl;
    return;
}
