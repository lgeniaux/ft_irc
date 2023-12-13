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
    int readFromClient(Client& client);
    Client& getClient(int client_fd);
    // Channel methods
    void createChannel(const std::string& name);
    void joinChannel(const std::string &name, std::string nickname);
    void leaveChannel(const std::string &name, std::string nickname);
    void handleChannelMessage(const std::string& channelName, const std::string& message, const std::string senderNick);
    void handlePrivateMessage(const std::string& recipientNick, const std::string& message, int sender_fd);
    Channel* getChannel(const std::string& name);
    void updateNicknameMap(const std::string& oldNick, const std::string& newNick, Client& client);
    friend class CommandHandler;  
    int getFdFromNickname(const std::string& nickname);

    
private:
    int port;
    std::string password;
    int server_fd;
    sockaddr_in address;
    CommandHandler* commandHandler;
    std::map<int, Client> clients;
    ssize_t readFromSocket(int client_fd, char *buffer, size_t size);
    std::map<std::string, Channel> channels; // name -> channel object
    std::map<std::string, Client *> nicknameToClientMap; // nickname -> client object
    RFC2812Handler rfcHandler;
};

#endif
