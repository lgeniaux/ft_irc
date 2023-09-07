#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Server {
public:
    Server(int port, const std::string& password);
    ~Server();
    void run();
    void acceptClient();
    void authenticateClient(int client_fd);
    void readFromClient(int client_fd);
    void broadcastMessage(const std::string& message);

private:
    int port;
    std::string password;
    int server_fd;
    sockaddr_in address;
    std::map<int, sockaddr_in> clients;
};

#endif
