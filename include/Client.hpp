#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <netinet/in.h> 
#include "Colors&Ascii.hpp"

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
    
    void setUsername(const std::string& name);
    std::string getUsername() const;

    void setRealname(const std::string& name);
    std::string getRealname() const;

    //LOGIN FLOW
    void setPassReceived(bool state);
    bool isPassReceived() const;

    void setNickReceived(bool state);
    bool isNickReceived() const;

    void setUserReceived(bool state);
    bool isUserReceived() const;

private:
    int fd;
    bool authenticated;
    std::string nickname;
    sockaddr_in address;
    std::string username;
    std::string realname;

    bool passReceived;
    bool nickReceived;
    bool userReceived;
};

#endif
