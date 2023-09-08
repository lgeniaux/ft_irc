#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <algorithm>
#include <sstream>
#include "Command.hpp"

Server::Server(int port, const std::string& password)
    : port(port), password(password), server_fd(-1) {
    commandHandler = new CommandHandler();
}

Server::~Server() {
    if (server_fd != -1) {
        close(server_fd);
    }
    delete commandHandler;
}

void Server::run() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return;
    }

    std::cout << "Server is running..." << std::endl;


    fd_set readfds;
    int max_sd;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            int sd = it->first;
            FD_SET(sd, &readfds);
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("select error");
        }

        if (FD_ISSET(server_fd, &readfds)) {
            acceptClient();
        }

        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            if (FD_ISSET(it->first, &readfds)) {
                readFromClient(it->second);
            }
        }
    }
}


void Server::acceptClient() {
    sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &addrlen);

    if (client_fd < 0) {
        std::cerr << "Failed to accept client" << std::endl;
        return;
    }

    Client newClient(client_fd, client_address);
    clients[client_fd] = newClient;

    std::cout << "Client connected: " << client_fd << std::endl;

    authenticateClient(client_fd);

}

void Server::authenticateClient(int client_fd) {
    char buffer[1024] = {0};
    ssize_t bytes_read = readFromSocket(client_fd, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        return;
    }
    
    std::string completeMessage(buffer, bytes_read);
    std::istringstream f(completeMessage);
    std::string line;
    std::cout << "Authenticating client : "<< client_fd << std::endl;
    while (std::getline(f, line)) {
        commandHandler->handleCommand(line, client_fd, *this);
    }
    if (clients[client_fd].isPassReceived() && clients[client_fd].isNickReceived() && clients[client_fd].isUserReceived()) {
        clients[client_fd].setAuthenticated(true);
        std::string auth_success = "Authentication success.\n";
        send(client_fd, auth_success.c_str(), auth_success.size(), 0);
    }
    //Debug :
    std::cout << "Client authenticated : " << clients[client_fd].isAuthenticated() << std::endl;
    std::cout << "Client pass received : " << clients[client_fd].isPassReceived() << std::endl;
    std::cout << "Client nick received : " << clients[client_fd].isNickReceived() << std::endl;
    std::cout << "Client user received : " << clients[client_fd].isUserReceived() << std::endl;
    
}



void Server::readFromClient(Client& client) {
    if (!client.isAuthenticated()) {
        return;
    }

    int client_fd = client.getFd();
    char buffer[1024] = {0};
    ssize_t bytes_read = readFromSocket(client_fd, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        return;
    }

    std::string message(buffer, bytes_read);
    //handle message as command
    commandHandler->handleCommand(message, client_fd, *this);

    std::cout << "Received message: " << message << std::endl;
    broadcastMessage(message, client_fd);
}

void Server::broadcastMessage(const std::string& message, int sender_fd) {
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        int client_fd = it->first;
        // Client& client = it->second; Pour l'instant pas besoin car tous les clients sont authentifiés

        // Ne pas envoyer le message au client qui l'a envoyé
        if (client_fd == sender_fd) {
            continue;
        }

        send(client_fd, message.c_str(), message.size(), 0);
    }
}

ssize_t Server::readFromSocket(int client_fd, char *buffer, size_t size) {
    ssize_t bytes_read = recv(client_fd, buffer, size, 0);
    if (bytes_read <= 0) {
        std::cerr << "Failed to read from client or client disconnected." << std::endl;
        close(client_fd);
        clients.erase(client_fd);
    }
    return bytes_read;
}

void Server::createChannel(const std::string& name) {
    channels[name] = Channel(name);
}


void Server::joinChannel(const std::string& name, int client_fd) {
    if (channels.find(name) == channels.end()) {
        std::cerr << "Channel " << name << " does not exist" << std::endl;
    }
    channels[name].addUser(client_fd);
    std::string response = "You have joined channel " + name;
    send(client_fd, response.c_str(), response.length(), 0);
}


void Server::leaveChannel(const std::string& name, int client_fd) {
    if (channels.find(name) != channels.end()) {
        channels[name].removeUser(client_fd);
    }
}

void Server::broadcastToChannel(const std::string& message, const std::string& channelName, int sender_fd) {
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        Channel& channel = it->second;
        std::set<int> users = channel.getUsers();
        for (std::set<int>::iterator userIt = users.begin(); userIt != users.end(); ++userIt) {
            int client_fd = *userIt;
            if (client_fd != sender_fd) {
                send(client_fd, message.c_str(), message.size(), 0);
            }
        }
    }
}


Channel& Server::getChannel(const std::string& name) {
    if (channels.find(name) == channels.end()) {
        std::cerr << "Channel " << name << " does not exist" << std::endl;
    }
    
    return channels[name];
}