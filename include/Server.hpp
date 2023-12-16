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

class Server
{
public:
    Server(int port, const std::string &password);
    ~Server();
    void run();
    void acceptClient();
    void authenticateClient(int client_fd);
    int readFromClient(Client &client);
    Client &getClient(int client_fd);
    // Channel methods
    void createChannel(const std::string &name);
    void joinChannel(const std::string &name, std::string nickname, Client &client);
    void leaveChannel(const std::string &name, std::string nickname);
    void handleChannelMessage(const std::string &channelName, const std::string &message, const std::string senderNick);
    Channel *getChannel(const std::string &name);
    void updateNicknameMap(const std::string &oldNick, const std::string &newNick, Client &client);
    void updateNickChannels(const std::string &oldNick, const std::string &newNick);
    void removeFdFromNicknameMap(int fd);
    friend class CommandHandler;
    int getFdFromNickname(const std::string &nickname);
    void markClientForDisconnection(int client_fd);
    void disconnectMarkedClients(fd_set &readfds);
    void broadcastMessageToUsers(const std::string &message, std::set<std::string> &users);
    //Returns a list of all users which have a channel in common with the client without the duplicates
    std::set<std::string> getCommonUsers(const std::string &nickname);
    void serverKick(const std::string channelName, int client_fd);
private:
    int port;
    std::string password;
    int server_fd;
    sockaddr_in address;
    CommandHandler *commandHandler;
    std::map<int, Client> clients;
    ssize_t readFromSocket(int client_fd, char *buffer, size_t size);
    std::map<std::string, Channel> channels;             // name -> channel object
    std::map<std::string, Client *> nicknameToClientMap; // nickname -> client object
    std::set<int> clientsToDisconnect;
    RFC2812Handler rfcHandler;
    std::map<int, std::string> partialCommands;
};

    // std::set<std::string> users;
    // std::set<std::string> operators;
    // std::map<std::string, Client *> nicknameToClientMap;

#endif
