#include "Command.hpp"
#include <sstream>
#include <iterator>
#include <tr1/unordered_map>

void CommandHandler::handleCommand(const std::string& command, int client_fd, std::map<int, std::string>& nicknames) {
    std::istringstream iss(command);
    std::istream_iterator<std::string> issIt(iss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> tokens;

    typedef void (*commandFunction)(const std::vector<std::string>&, int, std::map<int, std::string>&);

    std::tr1::unordered_map<std::string, commandFunction> commandRegistry;

    commandRegistry["NICK"] = handleNICK;
    commandRegistry["JOIN"] = handleJOIN;
    commandRegistry["PRIVMSG"] = handlePRIVMSG;

    for (; issIt != end; ++issIt) {
        tokens.push_back(*issIt);
    }
    
    if (tokens.empty()) {
        return;
    }

    std::string cmd = tokens[0];

    if (commandRegistry.find(cmd) != commandRegistry.end()) {
        commandRegistry[cmd](tokens, client_fd, nicknames);
        return;
    }
    std::cerr << "Unknown command: " << cmd << std::endl;
    return;
}
