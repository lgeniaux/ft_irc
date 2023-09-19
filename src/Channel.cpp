#include "Channel.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"

#include <iostream>

Channel::Channel(const std::string& name, const std::string& topic)
    : name(name), topic(topic) {
}

const std::string& Channel::getName() const {
    return name;
}

const std::string& Channel::getTopic() const {
    return topic;
}

void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

void Channel::addUser(int client_fd) {
    users.insert(client_fd);
}

void Channel::removeUser(int client_fd) {
    users.erase(client_fd);
}

bool Channel::hasUser(int client_fd) const {
    return users.find(client_fd) != users.end();
}

void Channel::addOperator(int client_fd) {
    operators.insert(client_fd);
}

void Channel::removeOperator(int client_fd) {
    operators.erase(client_fd);
}

bool Channel::isOperator(int client_fd) const {
    return operators.find(client_fd) != operators.end();
}

void Channel::setMode(char mode, bool enabled) {
    modes[mode] = enabled;
}

bool Channel::getMode(char mode) const {
    return modes.find(mode) != modes.end() ? modes.at(mode) : false;
}

std::set<int> Channel::getUsers() const {
    return users;
}

void Channel::inviteUser(int client_fd) {
    invitedUsers.insert(client_fd);
}

void Channel::removeInvite(int client_fd) {
    invitedUsers.erase(client_fd);
}

bool Channel::isInvited(int client_fd) const {
    return invitedUsers.find(client_fd) != invitedUsers.end();
}

void Channel::broadcastMessageToChannel(const std::string& message, int sender_fd) {
    std::string formattedMessage = RFC2812Handler::formatMessage(message);
    for (std::set<int>::iterator it = users.begin(); it != users.end(); ++it) {
        int client_fd = *it;
        if (client_fd == sender_fd) {
            continue;
        }
        send(client_fd, formattedMessage.c_str(), formattedMessage.size(), 0);
        //debug
        // std::cout << "Message sent to client " << client_fd << std::endl;
    }
}
