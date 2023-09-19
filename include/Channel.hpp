#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>

class Channel {
public:
    Channel() {}
    Channel(const std::string& name, const std::string& topic = "");

    const std::string& getName() const;
    const std::string& getTopic() const;
    void setTopic(const std::string& newTopic);

    void addUser(int client_fd);
    void removeUser(int client_fd);
    bool hasUser(int client_fd) const;

    void addOperator(int client_fd);
    void removeOperator(int client_fd);
    bool isOperator(int client_fd) const;

    void setMode(char mode, bool enabled);
    bool getMode(char mode) const;

    std::set<int> getUsers() const;

    void inviteUser(int client_fd);
    void removeInvite(int client_fd);
    bool isInvited(int client_fd) const;

    //MESSAGES
    void broadcastMessageToChannel(const std::string& message, int sender_fd);

private:
    std::string name;
    std::string topic;
    std::set<int> users;
    std::set<int> operators;
    std::map<char, bool> modes; // 'i', 't', 'k', 'o', 'l' ...
    std::set<int> invitedUsers;
};

#endif // CHANNEL_HPP
