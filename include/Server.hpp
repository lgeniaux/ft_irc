#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Command.hpp"
#include "Client.hpp"

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();
    void run();
    void acceptClient();
    void authenticateClient(int client_fd);
    void readFromClient(Client& client);
    void broadcastMessage(const std::string& message, int sender_fd);

private:
    int port;
    std::string password;
    int server_fd;
    sockaddr_in address;
    CommandHandler commandHandler;
    std::map<int, Client> clients;
};

#endif
