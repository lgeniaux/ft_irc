#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <netinet/in.h> 

class Client {
public:
    Client();
    Client(int fd, const sockaddr_in& clientAddress);
    bool isAuthenticated() const;
    void setAuthenticated(bool auth);
    int getFd() const;
    void setNickname(const std::string& name);
    std::string getNickname() const;
    sockaddr_in getAddress() const;

private:
    int fd;
    bool authenticated;
    std::string nickname;
    sockaddr_in address;
};

#endif
