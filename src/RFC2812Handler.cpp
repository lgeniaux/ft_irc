#include "RFC2812Handler.hpp"
#include <sstream>
#include <unistd.h>

void RFC2812Handler::sendInitialConnectionMessages(Client& client) {
    int fd = client.getFd();
    // Add more initial messages here as needed, per the RFC 2812 spec
    std::string welcomeMessage = formatMessage(":Welcome to the IRC Server " + client.getNickname());
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
