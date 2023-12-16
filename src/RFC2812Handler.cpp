#include "RFC2812Handler.hpp"
#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

void RFC2812Handler::sendInitialConnectionMessages(Client &client)
{
    int fd = client.getFd();
    std::string welcomeMessage = ":localhost 001 " + client.getNickname() + " :" + WELCOME_MSG;
    if (welcomeMessage.find("${NAME}") != std::string::npos)
        welcomeMessage.replace(welcomeMessage.find("${NAME}"), 7, client.getNickname());
    if (welcomeMessage.find("${USER}") != std::string::npos)
        welcomeMessage.replace(welcomeMessage.find("${USER}"), 7, client.getUsername());
    if (welcomeMessage.find("${HOST}") != std::string::npos)
        welcomeMessage.replace(welcomeMessage.find("${HOST}"), 7, inet_ntoa(client.getAddress().sin_addr));
    welcomeMessage = formatMessage(welcomeMessage);
    send(fd, welcomeMessage.c_str(), welcomeMessage.size(), 0);
}

void RFC2812Handler::sendResponse(int code, Client &client, const std::string &message)
{
    int fd = client.getFd();
    std::ostringstream oss;
    if (code == 482 || code == 461 || code == 442 || code == 441 || code == 331)
        oss << ":localhost " << code << " " << message;
    else
        oss << ":localhost " << code << " " << client.getNickname() << " " << message;
    std::string formattedMessage = formatMessage(oss.str());
    send(fd, formattedMessage.c_str(), formattedMessage.size(), 0);
    while (!formattedMessage.empty() &&
           (formattedMessage[formattedMessage.size() - 1] == '\r' ||
            formattedMessage[formattedMessage.size() - 1] == '\n'))
    {
        formattedMessage.erase(formattedMessage.size() - 1);
    }
    std::cout << LIGHT BLUE << "Sending :" << RESET GRAY << formattedMessage << RESET << std::endl;
}

std::string RFC2812Handler::formatMessage(const std::string &message)
{
    return message + "\r\n";
}
