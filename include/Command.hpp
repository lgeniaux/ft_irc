#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "Server.hpp"


class CommandHandler {
    public:
        void handleCommand(const std::string& command, int client_fd, Server& server);
    private:
        static void handleJOIN(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleKICK(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleINVITE(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleTOPIC(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleMODE(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handlePASS(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleNICK(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleUSER(const std::vector<std::string>& tokens, int client_fd, Server& server);

};

#endif
