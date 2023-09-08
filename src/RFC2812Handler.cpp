#include "RFC2812Handler.hpp"
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>

void RFC2812Handler::sendInitialConnectionMessages(Client& client) {
    int fd = client.getFd();
    std::ostringstream oss;
    oss << ":localhost 001 " << client.getNickname() 
        << " :Welcome to the Internet Relay Network " << client.getNickname() 
        << "!" << client.getUsername() << "@" << inet_ntoa(client.getAddress().sin_addr);
    std::string welcomeMessage = formatMessage(oss.str());
    send(fd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
}


void RFC2812Handler::sendResponse(int code, Client& client, const std::string& message) {
    int fd = client.getFd();
    std::ostringstream oss;
    oss << ":" << code << " " << client.getNickname() << " " << message;
    std::string formattedMessage = formatMessage(oss.str());
    send(fd, formattedMessage.c_str(), formattedMessage.size(), 0);
}

std::string RFC2812Handler::formatMessage(const std::string& message) {
    return message + "\r\n";
}
