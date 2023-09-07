#include "Server.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <cstdio>

Server::Server(int port, const std::string& password)
    : port(port), password(password), server_fd(-1) {
}

Server::~Server() {
    if (server_fd != -1) {
        close(server_fd);
    }
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
    std::string authMessage = "Enter password: ";
    send(client_fd, authMessage.c_str(), authMessage.size(), 0);
    
    char buffer[1024] = {0};
    ssize_t bytes_read = readFromSocket(client_fd, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
        return;
    }

    std::string received_password(buffer, bytes_read);
    std::cout << "Authenticating client: " << client_fd << std::endl;

    if (!received_password.empty() && received_password[received_password.size() - 1] == '\n') {
        received_password.erase(received_password.size() - 1);
    }
    std::cout << "Received password: [" << received_password << "]" << std::endl;
    std::cout << "Expected password: [" << password << "]" << std::endl;

    if (received_password == password) {
        std::string auth_success = "Authentication success.\n";
        std:: cout << auth_success.substr(0, auth_success.length() - 2)  << " for client :" << client_fd << std::endl; 
        send(client_fd, auth_success.c_str(), auth_success.size(), 0);
        clients[client_fd].setAuthenticated(true);
    }else{
        std::string auth_failed = "Authentication failed.\n";
        std:: cout << auth_failed.substr(0, auth_failed.length() - 2) << " for client :" << client_fd << std::endl; 
        send(client_fd, auth_failed.c_str(), auth_failed.size(), 0);
        close(client_fd);
        clients.erase(client_fd);
        return;
    }
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