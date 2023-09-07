#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>

class CommandHandler {
public:
    void handleCommand(const std::string& command, int client_fd, std::map<int, std::string>& nicknames);
private:
    void static handleNICK(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames);
    void static handleJOIN(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames);
    void static handlePRIVMSG(const std::vector<std::string>& tokens, int client_fd, std::map<int, std::string>& nicknames);
};

#endif
