#include "Server.hpp"
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <Channel.hpp>
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }


    int port = atoi(argv[1]);
    std::string password = argv[2];

    Server server(port, password);
    server.run();

    //Create a default channel without invite
    server.createChannel("#default");
    server.getChannel("#default").setMode('i', false);

    return 0;
}
