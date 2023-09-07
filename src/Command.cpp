#include "Command.hpp"
#include <iostream>
#include <sstream>
#include <iterator>

void CommandHandler::handleCommand(const std::string& command, int client_fd, std::map<int, std::string>& nicknames) {
    std::istringstream iss(command);
    std::istream_iterator<std::string> issIt(iss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> tokens;

    for (; issIt != end; ++issIt) {
        tokens.push_back(*issIt);
    }

    if (tokens.empty()) {
        return;
    }

    std::string cmd = tokens[0];

    if (cmd == "NICK") {
        if (tokens.size() != 2) {
            std::cerr << "Invalid NICK command format." << std::endl;
            return;
        }
        handleNICK(tokens, client_fd, nicknames);
    } else if (cmd == "JOIN") {
        if (tokens.size() != 2) {
            std::cerr << "Invalid JOIN command format." << std::endl;
            return;
        }
        handleJOIN(tokens, client_fd, nicknames);
    } else if (cmd == "PRIVMSG") {
        if (tokens.size() < 3) {
            std::cerr << "Invalid PRIVMSG command format." << std::endl;
            return;
        }
        handlePRIVMSG(tokens, client_fd, nicknames);
    }
}

void CommandHandler::handleNICK(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames) {
    std::string nickname = tokens[1];
    nicknames[client_fd] = nickname;
    std::cout << "Nickname set to " << nickname << " for client " << client_fd << std::endl;
}

void CommandHandler::handleJOIN(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames) {
    // TO DO
    //use nicknames to avoid "unused parameter"
    nicknames[client_fd] = nicknames[client_fd];
    std::cout << "Client " << client_fd << " wants to join channel " << tokens[1] << std::endl;
}

void CommandHandler::handlePRIVMSG(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames) {
    // TO DO
    //use nicknames to avoid "unused parameter"
    nicknames[client_fd] = nicknames[client_fd];
    std::cout << "Private message from client " << client_fd << " to " << tokens[1] << std::endl;
}
