#include "Client.hpp"
#include <cstring>

Client::Client() : fd(-1), authenticated(false), passReceived(false), nickReceived(NOT_RECEIVED), userReceived(false)
{
    memset(&address, 0, sizeof(address));
}

Client::Client(int fd, const sockaddr_in &clientAddress)
    : fd(fd), authenticated(false), address(clientAddress), passReceived(false), nickReceived(false), userReceived(false)
{
}

bool Client::isAuthenticated() const
{
    return authenticated;
}

void Client::setAuthenticated(bool auth)
{
    authenticated = auth;
}

int Client::getFd() const
{
    return fd;
}

void Client::setNickname(const std::string &name)
{
    nickname = name;
}

std::string Client::getNickname() const
{
    return nickname;
}

sockaddr_in Client::getAddress() const
{
    return address;
}

void Client::setUsername(const std::string &name)
{
    username = name;
}

std::string Client::getUsername() const
{
    return username;
}

void Client::setRealname(const std::string &name)
{
    realname = name;
}

std::string Client::getRealname() const
{
    return realname;
}

void Client::setPassReceived(bool state)
{
    passReceived = state;
}

bool Client::isPassReceived() const
{
    return passReceived;
}

void Client::setNickReceived(char state)
{
    nickReceived = state;
}

char Client::getNickReceived() const
{
    return nickReceived;
}

void Client::setUserReceived(bool state)
{
    userReceived = state;
}

bool Client::isUserReceived() const
{
    return userReceived;
}
