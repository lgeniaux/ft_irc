#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "RFC2812Handler.hpp"

class CommandHandler;

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();
    void run();
    void acceptClient();
    void authenticateClient(int client_fd);
    void readFromClient(Client& client);
    void broadcastMessage(const std::string& message, int sender_fd);
    Client& getClient(int client_fd);
    // Channel methods
    void createChannel(const std::string& name);
    void joinChannel(const std::string& name, int client_fd);
    void leaveChannel(const std::string& name, int client_fd);
    void broadcastToChannel(const std::string& message, const std::string& channelName, int sender_fd);
    Channel* getChannel(const std::string& name);
    friend class CommandHandler;  
    
private:
    int port;
    std::string password;
    int server_fd;
    sockaddr_in address;
    CommandHandler* commandHandler;
    std::map<int, Client> clients;
    ssize_t readFromSocket(int client_fd, char *buffer, size_t size);
    std::map<std::string, Channel> channels; // name -> channel object
    RFC2812Handler rfcHandler;
};

#endif
