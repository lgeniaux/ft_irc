#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <algorithm>
#include <sstream>
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

    // Prevent "Bind failed" error message because of socket in TIME_WAIT state
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Failed to set socket option SO_REUSEADDR" << std::endl;
        close(server_fd);
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << ERROR << "Bind failed" << std::endl;
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
    }
}

void Server::acceptClient()
{
    sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);

    if (client_fd < 0)
    {
        std::cerr << ERROR << "Failed to accept client" << std::endl;
        return;
    }

    Client newClient(client_fd, client_address);
    clients[client_fd] = newClient;

    std::cout << GREEN << "Client connected: " << RESET << client_fd - 3 << std::endl;

    authenticateClient(client_fd);
}

void Server::authenticateClient(int client_fd)
{
    char buffer[1024] = {0};
    ssize_t bytes_read = readFromSocket(client_fd, buffer, sizeof(buffer));
    if (bytes_read <= 0)
    {
        std::cout << RED << "Client disconnected: " << client_fd - 3 << RESET << std::endl;
        return;
    }

    std::string completeMessage(buffer, bytes_read);
    std::istringstream f(completeMessage);
    std::string line;
    std::cout << LIGHT GRAY << "[" << client_fd - 3 << "] Authenticating client" << RESET << std::endl;
    // debug print the data the user is sending
    std::string debugMessage = completeMessage;
    while (debugMessage.find("\r\n") != std::string::npos)
        debugMessage.replace(debugMessage.find("\r\n"), 2, LIGHT PURPLE "\\r\\n" RESET);
    std::cout << LIGHT GRAY << "[" << client_fd - 3 << "] Data received : " << RESET << debugMessage << std::endl;

    while (std::getline(f, line))
    {
        commandHandler->handleCommand(line, client_fd, *this);
    }
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
    if (clients.find(client_fd) != clients.end())
    {
        std::cout << "Client socket is still open" << RESET << std::endl;
    }
    else
    {
        std::cout << "Client socket is closed" << std::endl;
    }
}

int Server::readFromClient(Client &client)
{

    if (!client.isAuthenticated())
    {
        // client is not authenticated yet, we listen to him again to get all the command (PASS, NICK, USER)
        authenticateClient(client.getFd());
        return 0;
    }
    else
    {
        std::cout << LIGHT GRAY << "[" << client.getFd() - 3 << "] Reading from authenticated client" << RESET << std::endl; // Debug line
        int client_fd = client.getFd();
        char buffer[1024] = {0};
        ssize_t bytes_read = readFromSocket(client_fd, buffer, sizeof(buffer));
        if (bytes_read <= 0)
        {
            close(client_fd);
            return -1;
        }

        std::string message(buffer, bytes_read);
        // handle message as command
        commandHandler->handleCommand(message, client_fd, *this);

        std::string debugMessage = message;
        while (debugMessage.find("\r\n") != std::string::npos)
            debugMessage.replace(debugMessage.find("\r\n"), 2, LIGHT PURPLE "\\r\\n" RESET);
        std::cout << LIGHT GRAY << "Received message: " << RESET << debugMessage << std::endl;
        if (clients.find(client_fd) != clients.end())
        {
            std::cout << LIGHT GRAY << "Client socket is still open" << std::endl;
        }
        else
        {
            std::cout << "Client socket is closed" << std::endl;
        }
        return 0;
    }
}

ssize_t Server::readFromSocket(int client_fd, char *buffer, size_t size)
{
    ssize_t bytes_read = recv(client_fd, buffer, size, 0);
    std::cout << LIGHT GRAY << "[" << client_fd - 3 << "] bytes read: " << bytes_read << RESET << std::endl; // Debug line
    if (bytes_read <= 0)
    {
        std::cerr << ERROR << "Failed to read from client or client disconnected." << std::endl;
        close(client_fd);
        clients.erase(client_fd);
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
    std::cout << "User " << nickname << " joined channel " << name << std::endl;
    RFC2812Handler::sendResponse(332, getClient(getFdFromNickname(nickname)), name + " :" + channels[name].getTopic());
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

Channel *Server::getChannel(const std::string &name)
{
    if (channels.find(name) == channels.end())
    {
        std::cerr << INFO << "Channel " << name << " does not exist" << std::endl;
        return NULL;
    }

    return &channels[name];
}

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
}

int Server::getFdFromNickname(const std::string& nickname) {
    if(nicknameToClientMap.find(nickname) != nicknameToClientMap.end()) {
        return nicknameToClientMap[nickname]->getFd();
    }
    return -1;
}
