#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <ctime>
#include <set>
#include <map>

class Server;

class Channel
{
public:
    Channel() {}
    Channel(const std::string &name, const std::string &topic = "", const std::time_t &topicTime = 0);

    const std::string &getName() const;
    const std::string &getTopic() const;
    const std::time_t &getTopicTime() const;
    void setTopic(const std::string &newTopic);
    void setTopicTime(const std::time_t &newTopicTime);

    void addUser(std::string nickname);
    void removeUser(std::string nickname);
    bool hasUser(std::string nickname) const;

    void addOperator(std::string nickname);
    void removeOperator(std::string nickname);
    bool isOperator(std::string nickname) const;

    void setMode(char mode, bool enabled);
    bool getMode(char mode) const;

    bool isInChannel(std::string nickname) const;

    void inviteUser(std::string nickname);
    void removeInvite(std::string nickname);
    bool isInvited(std::string nickname) const;

    void setKey(std::string newKey);
    bool checkKey(std::string key) const;
    std::string getKey() const;

    void updateNickname(std::string oldNick, std::string newNick);

    void setLimit(int newLimit);
    size_t getLimit() const;
    // MESSAGES
    void broadcastMessageToChannel(const std::string &message, Server &server, const std::string &sender);
    std::string getModes() const;

    std::set<std::string> getUsers() const;
    void updateOperatorNickname(std::string oldNick, std::string newNick);

private:
    std::string name;
    std::string topic;
    time_t topicTime;
    std::string key;
    size_t limit;
    std::set<std::string> users;
    std::set<std::string> operators;
    std::set<std::string> invitedUsers;
    std::map<char, bool> modes; // 'i', 't', 'k', 'o', 'l' ...
};

#endif // CHANNEL_HPP
