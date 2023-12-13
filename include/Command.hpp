#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <map>
#include <vector>
#include <ctime>
#include <iostream>
#include "Server.hpp"
#include "RFC2812Handler.hpp"

class CommandHandler {
    public:
        void handleCommand(const std::string& command, int client_fd, Server& server);
    private:
        static void handleKICK(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleINVITE(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleTOPIC(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleMODE(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handlePASS(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleNICK(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleUSER(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleCAP(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleJOIN(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handlePRIVMSG(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handlePART(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handleQUIT(const std::vector<std::string>& tokens, int client_fd, Server& server);
        static void handlePING(const std::vector<std::string>& tokens, int client_fd, Server& server);
};

#endif
