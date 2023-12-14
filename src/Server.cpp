#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <algorithm>
#include <sstream>
#include <fcntl.h>
#include "Command.hpp"
#include "RFC2812Handler.hpp"

Server::Server(int port, const std::string &password)
    : port(port), password(password), server_fd(-1)
{
    commandHandler = new CommandHandler();
}

Server::~Server()
{
    if (server_fd != -1)
    {
        close(server_fd);
    }
    delete commandHandler;
}

void Server::run()
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << ERROR << "Failed to create socket" << std::endl;
        return;
    }
    // Set SO_REUSEADDR to allow immediate reuse of the port and avoid the "Address already in use" error
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << ERROR << "Failed to set SO_REUSEADDR" << std::endl;
        close(server_fd);
        return;
    }

    // Set the server socket to non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags < 0)
    {
        std::cerr << ERROR << "Failed to get flags for socket" << std::endl;
        return;
    }

    flags |= O_NONBLOCK;
    if (fcntl(server_fd, F_SETFL, flags) < 0)
    {
        std::cerr << ERROR << "Failed to set server socket to non-blocking" << std::endl;
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int tries = 0;
    while (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << ERROR << "Bind failed " << tries << "/30" << std::endl;
        usleep(1000000);
        if (tries++ >= 30)
            return;
    }

    if (listen(server_fd, 3) < 0)
    {
        std::cerr << ERROR << "Listen failed" << std::endl;
        return;
    }

    std::cout << INFO << "Server is running..." << std::endl;

    fd_set readfds;
    int max_sd;

    while (true)
    {
        std::cout << LIGHT GRAY << "Server loop iteration" << RESET << std::endl;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
            int sd = it->first;
            FD_SET(sd, &readfds);
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            perror("select error");
        }

        if (FD_ISSET(server_fd, &readfds))
        {
            acceptClient();
        }

        std::vector<int> clients_to_remove;           // To collect clients that should be removed
        std::map<int, Client> temp_clients = clients; // Create a temporary copy of the clients map

        for (std::map<int, Client>::iterator it = temp_clients.begin(); it != temp_clients.end(); ++it)
        {
            if (FD_ISSET(it->first, &readfds))
            {
                if (readFromClient(clients[it->first]) == -1) // Remove client if -1 is returned
                {
                    clients_to_remove.push_back(it->first);
                }
            }
        }

        // Actually remove the clients
        for (std::vector<int>::iterator it = clients_to_remove.begin(); it != clients_to_remove.end(); ++it)
        {
            std::string oldNick = clients[*it].getNickname();
            nicknameToClientMap.erase(oldNick);
            clients.erase(*it);
        }
        disconnectMarkedClients(readfds);
    }
}

void Server::acceptClient()
{
    sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);

    if (client_fd < 0)
    {
        // Non-blocking mode
        if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            return; // Not an error, just no connections to accept
        }
        else
        {
            std::cerr << ERROR << "Failed to accept client" << std::endl;
            return;
        }
    }

    // Set the new client socket to non-blocking mode
    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags < 0)
    {
        std::cerr << ERROR << "Failed to get flags for client socket" << std::endl;
        close(client_fd);
        return;
    }

    flags |= O_NONBLOCK;
    if (fcntl(client_fd, F_SETFL, flags) < 0)
    {
        std::cerr << ERROR << "Failed to set client socket to non-blocking" << std::endl;
        close(client_fd);
        return;
    }

    Client newClient(client_fd, client_address);
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        std::cout << it->first << " => " << it->second.getNickname() << std::endl;
    }

    clients[client_fd] = newClient;

    std::cout << GREEN << "Client connected: " << RESET << client_fd - 3 << std::endl;

    authenticateClient(client_fd);
}

void Server::authenticateClient(int client_fd)
{
    char buffer[1024] = {0};
    ssize_t bytes_read = readFromSocket(client_fd, buffer, sizeof(buffer));
    if (bytes_read == -1)
    {
        return; // Client disconnected
    }
    else if (bytes_read == -2)
    {
        return; // No data available yet, return and wait for more data
    }

    // Append new data to the existing partial command for this client
    partialCommands[client_fd] += std::string(buffer, bytes_read);

    // Check if a complete command (ending with "\r\n") is present
    size_t endPos;
    while ((endPos = partialCommands[client_fd].find("\r\n")) != std::string::npos)
    {
        // Extract the complete command
        std::string completeCommand = partialCommands[client_fd].substr(0, endPos);

        std::string debugCommand = completeCommand;
        std::replace(debugCommand.begin(), debugCommand.end(), '\r', '|');
        std::replace(debugCommand.begin(), debugCommand.end(), '\n', '|');
        std::cout << LIGHT GRAY << "[" << client_fd - 3 << "] Command received : " << RESET << debugCommand << std::endl;

        // Process the complete command
        std::istringstream f(completeCommand);
        std::string line;
        while (std::getline(f, line))
        {
            commandHandler->handleCommand(line, client_fd, *this);
        }

        partialCommands[client_fd] = partialCommands[client_fd].substr(endPos + 2);
    }

    // Authentication check
    if (clients[client_fd].isPassReceived() && clients[client_fd].isNickReceived() && clients[client_fd].isUserReceived())
    {
        clients[client_fd].setAuthenticated(true);
        RFC2812Handler::sendInitialConnectionMessages(clients[client_fd]);
    }
    // Debug :
    if (clients[client_fd].isAuthenticated())
        std::cout << "[" << client_fd - 3 << "] " << GREEN << "Client authenticated" << RESET << " | ";
    else
        std::cout << "[" << client_fd - 3 << "] " << RED << "Client not authenticated" << RESET << " | ";
    if (clients[client_fd].isPassReceived())
        std::cout << GREEN << "PASS received" << RESET << " | ";
    else
        std::cout << RED << "PASS not received" << RESET << " | ";
    if (clients[client_fd].isNickReceived())
        std::cout << GREEN << "NICK received" << RESET << " | ";
    else
        std::cout << RED << "NICK not received" << RESET << " | ";
    if (clients[client_fd].isUserReceived())
        std::cout << GREEN << "USER received" << RESET << std::endl;
    else
        std::cout << RED << "USER not received" << RESET << std::endl;
    if (clients.find(client_fd) == clients.end())
        std::cout << "Client socket is closed" << std::endl;
}

int Server::readFromClient(Client &client)
{
    if (!client.isAuthenticated())
    {
        authenticateClient(client.getFd());
        return 0;
    }
    else
    {
        std::cout << LIGHT GRAY << "[" << client.getFd() - 3 << "] Reading from authenticated client" << RESET << std::endl;
        int client_fd = client.getFd();
        char buffer[1024] = {0};
        ssize_t bytes_read = readFromSocket(client_fd, buffer, sizeof(buffer));

        if (bytes_read <= 0)
        {
            return -1; // Client disconnected or error
        }

        // Append new data to the existing partial command for this client
        partialCommands[client_fd] += std::string(buffer, bytes_read);

        // Check if a complete command (ending with "\r\n") is present
        size_t endPos;
        while ((endPos = partialCommands[client_fd].find("\r\n")) != std::string::npos)
        {
            std::string completeCommand = partialCommands[client_fd].substr(0, endPos);

            commandHandler->handleCommand(completeCommand, client_fd, *this);

            partialCommands[client_fd] = partialCommands[client_fd].substr(endPos + 2);
        }

        return 0;
    }
}

ssize_t Server::readFromSocket(int client_fd, char *buffer, size_t size)
{
    ssize_t bytes_read = recv(client_fd, buffer, size, 0);

    if (bytes_read <= 0)
    {
        std::cout << LIGHT GRAY << "[" << client_fd - 3 << "] bytes read: " << bytes_read << RESET << std::endl;
        if (bytes_read == 0)
        {
            // Normal disconnection
            std::cout << RED << "Client disconnected: " << client_fd - 3 << RESET << std::endl;
            CommandHandler commandHandler;
            commandHandler.handleCommand("QUIT :Remote host closed the connection\r\n", client_fd, *this);
            // close(client_fd);
            // clients.erase(client_fd); // already done in handleQUIT
            return -1;
        }
        else if (errno == EWOULDBLOCK || errno == EAGAIN)
        {
            // No data available yet
            return -2;
        }
        else
        {
            // Other errors
            std::cerr << ERROR << "Failed to read from client or client disconnected." << std::endl;
            close(client_fd);
            clients.erase(client_fd);
            return -1;
        }
    }

    return bytes_read;
}

void Server::createChannel(const std::string &name)
{
    channels[name] = Channel(name);
}

void Server::joinChannel(const std::string &name, std::string nickname)
{
    if (channels.find(name) == channels.end())
    {
        // In most IRC servers if a user tries to join an unexisting channel, it is created. The user is also made operator of the channel.
        createChannel(name);
        channels[name].addOperator(nickname);
    }
    channels[name].addUser(nickname);
    // send a RFC2812 message to the client to inform him that he joined the channel
    if (!channels[name].getTopic().empty())
    {
        RFC2812Handler::sendResponse(332, getClient(getFdFromNickname(nickname)), name + " " + channels[name].getTopic());

        // Use stringstream for number to string conversion because of c++98
        std::ostringstream ss;
        ss << channels[name].getTopicTime();
        std::string topicTimeStr = ss.str();

        RFC2812Handler::sendResponse(333, getClient(getFdFromNickname(nickname)), name + " " + nickname + " " + topicTimeStr);
    }
    channels[name].broadcastMessageToChannel(":" + nickname + " JOIN :" + name + "\r\n", *this, nickname);
}

void Server::leaveChannel(const std::string &name, std::string nickname)
{
    if (channels.find(name) != channels.end())
    {
        channels[name].removeUser(nickname);
    }
}

void Server::handleChannelMessage(const std::string &channelName, const std::string &message, std::string senderNick)
{
    // DEBUG
    // std::cout << "Handling channel message" << std::endl;
    // std::cout << "Channel name: " << channelName << std::endl;
    // std::cout << "Message: " << message << std::endl;
    // list all existing channel for debug
    // std::cout << "Existing channels: " << std::endl;
    if (channels.find(channelName) != channels.end())
    {
        channels[channelName].broadcastMessageToChannel(message, *this, senderNick);
        std::cout << "Message broadcasted to channel" << channelName << std::endl;
    }
    else
        std::cout << "Channel does not exist" << std::endl;
}

/**
 * @brief Returns a pointer to the channel with the given name.
 * If the channel does not exist, returns NULL.
 *
 * @param name: The name of the channel to get
 * @return Channel*
 */
Channel *Server::getChannel(const std::string &name)
{
    if (channels.find(name) == channels.end())
    {
        std::cerr << INFO << "Channel " << name << " does not exist" << std::endl;
        return NULL;
    }

    return &channels[name];
}

/**
 * @brief Returns a pointer to the client with the given fd.
 * If the client does not exist, returns garbage.
 *
 * @param client_fd: The fd of the client to get
 * @return Client*
 */
Client &Server::getClient(int client_fd)
{
    return clients[client_fd];
}

void Server::updateNicknameMap(const std::string &oldNick, const std::string &newNick, Client &client)
{
    if (!oldNick.empty())
    {
        nicknameToClientMap.erase(oldNick);
    }

    nicknameToClientMap[newNick] = &client;
    std::map<std::string, Channel>::iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
        if (it->second.isInChannel(oldNick))
            it->second.updateNickname(oldNick, newNick);
}

int Server::getFdFromNickname(const std::string &nickname)
{
    if (nicknameToClientMap.find(nickname) != nicknameToClientMap.end())
    {
        return nicknameToClientMap[nickname]->getFd();
    }
    return -1;
}

void Server::removeFdFromNicknameMap(int fd)
{
    std::map<std::string, Client *>::iterator it;
    for (it = nicknameToClientMap.begin(); it != nicknameToClientMap.end(); ++it)
    {
        if (it->second->getFd() == fd)
        {
            nicknameToClientMap.erase(it);
            return;
        }
    }
}

void Server::markClientForDisconnection(int client_fd)
{
    removeFdFromNicknameMap(client_fd);
    clientsToDisconnect.insert(client_fd);
}

void Server::disconnectMarkedClients(fd_set &readfds)
{
    for (std::set<int>::iterator it = clientsToDisconnect.begin(); it != clientsToDisconnect.end(); ++it)
    {
        int client_fd = *it;
        close(client_fd);
        clients.erase(client_fd);
        FD_CLR(client_fd, &readfds);
    }
    clientsToDisconnect.clear();
}