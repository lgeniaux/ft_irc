#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"
#include <unistd.h>

void CommandHandler::handlePART(const std::vector<std::string>& tokens, int client_fd, Server& server) {
	std::pair<Channel*, Client> preCheck;
	Channel *channel;
	Client  client;

	if (tokens.size() < 2) {
		RFC2812Handler::sendResponse(461, server.clients[client_fd], "PART :Not enough parameters");
		return;
	}
	preCheck = preChecks(tokens[1], client_fd, server, false);
	channel = preCheck.first;
	client = preCheck.second;
	if (channel == NULL)
		return;
	std::string nickname = client.getNickname();
	std::string message = "";
	if (tokens.size() > 2 && tokens[2] != ":")
		message = " " + tokens[2];
	channel->broadcastMessageToChannel(":" + nickname + " PART " + channel->getName() + message + "\r\n", server, "");
	server.leaveChannel(channel->getName(), nickname);
}
