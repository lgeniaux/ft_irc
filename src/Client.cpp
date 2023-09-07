#include "Client.hpp"
#include <cstring>

Client::Client() : fd(-1), authenticated(false) {
    memset(&address, 0, sizeof(address));
}

Client::Client(int fd, const sockaddr_in& clientAddress)
    : fd(fd), authenticated(false), address(clientAddress) {
}
bool Client::isAuthenticated() const {
    return authenticated;
}

void Client::setAuthenticated(bool auth) {
    authenticated = auth;
}

int Client::getFd() const {
    return fd;
}

void Client::setNickname(const std::string& name) {
    nickname = name;
}

std::string Client::getNickname() const {
    return nickname;
}

sockaddr_in Client::getAddress() const {
    return address;
}
