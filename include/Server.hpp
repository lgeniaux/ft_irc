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

#define BOLD	"\033[1m"
#define LIGHT	"\033[2m"
#define GRAY	"\033[90m"
#define RED		"\033[31m"
#define PURPLE	"\033[35m"
#define CYAN	"\033[36m"
#define GREEN	"\033[32m"
#define RESET	"\033[0m"

# define ERROR	BOLD RED "[Error] " RESET
# define WARN	BOLD PURPLE "[Warning] " RESET
# define INFO	BOLD CYAN "[Info] " RESET

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
