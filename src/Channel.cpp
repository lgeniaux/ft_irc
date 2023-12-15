#include "Channel.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"

#include <iostream>

Channel::Channel(const std::string &name, const std::string &topic, const std::time_t &topicTime)
    : name(name), topic(topic), topicTime(topicTime), key(""), limit(0)
{
}

const std::string &Channel::getName() const
{
    return name;
}

const std::string &Channel::getTopic() const
{
    return topic;
}

const std::time_t &Channel::getTopicTime() const
{
    return topicTime;
}

void Channel::setTopic(const std::string &newTopic)
{
    topic = newTopic;
}

void Channel::setTopicTime(const std::time_t &newTopicTime)
{
    topicTime = newTopicTime;
}

void Channel::addUser(std::string nickname)
{
    users.insert(nickname);
}

void Channel::removeUser(std::string nickname)
{
    users.erase(nickname);
}

bool Channel::hasUser(std::string nickname) const
{
    return users.find(nickname) != users.end();
}

void Channel::addOperator(std::string nickname)
{
    operators.insert(nickname);
}

void Channel::removeOperator(std::string nickname)
{
    operators.erase(nickname);
}

bool Channel::isOperator(std::string nickname) const
{
    return operators.find(nickname) != operators.end();
}

void Channel::setMode(char mode, bool enabled)
{
    modes[mode] = enabled;
}

bool Channel::getMode(char mode) const
{
    return modes.find(mode) != modes.end() ? modes.at(mode) : false;
}

void Channel::inviteUser(std::string nickname)
{
    invitedUsers.insert(nickname);
}

void Channel::removeInvite(std::string nickname)
{
    invitedUsers.erase(nickname);
}

bool Channel::isInvited(std::string nickname) const
{
    return invitedUsers.find(nickname) != invitedUsers.end();
}

void Channel::broadcastMessageToChannel(const std::string &message, Server &server, const std::string &sender)
{
    std::set<std::string>::iterator it;
    for (it = users.begin(); it != users.end(); ++it)
    {
        if (*it != sender)
        {
            std::string nickname = *it;
            int fd = server.getFdFromNickname(nickname);
            std::cout << LIGHT GRAY << "Sending message " << RESET << message << LIGHT GRAY << " to " << nickname << " on fd " << fd << RESET << std::endl;
            if (fd != -1)
            { // Check if fd is valid
                send(fd, message.c_str(), message.length(), 0);
            }
        }
    }
}

bool Channel::isInChannel(std::string nickname) const
{
    if (users.find(nickname) != users.end())
    {
        return true;
    }
    return false;
}
void Channel::setKey(std::string newKey)
{
    key = newKey;
}

std::string Channel::getKey() const
{
    return key;
}

bool Channel::checkKey(std::string key) const
{
    if (this->key == key)
    {
        return true;
    }
    return false;
}

void Channel::setLimit(int newLimit)
{
    limit = newLimit;
}

size_t Channel::getLimit() const
{
    return limit;
}

std::set<std::string> Channel::getUsers() const
{
    return users;
}

void Channel::updateNickname(std::string oldNick, std::string newNick)
{
    users.erase(oldNick);
    users.insert(newNick);
    if (operators.find(oldNick) != operators.end())
        updateOperatorNickname(oldNick, newNick);
}

void Channel::updateOperatorNickname(std::string oldNick, std::string newNick)
{
    operators.erase(oldNick);
    operators.insert(newNick);
}

//getModes (return a string with all the modes of the channel (like +nt for example)
std::string Channel::getModes() const
{
    std::string modes;
    if (getMode('t'))
    {
        modes += 't';
    }
    if (getMode('i'))
    {
        modes += 'i';
    }
    if (getMode('k'))
    {
        modes += 'k';
    }
    if (getMode('l'))
    {
        modes += 'l';
    }
    return modes;
} 