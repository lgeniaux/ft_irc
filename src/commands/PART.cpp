#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"
#include <unistd.h>

void PART(std::string nickname, std::string channel, std::string message, Server& server);

void CommandHandler::handlePART(const std::vector<std::string>& tokens, int client_fd, Server& server) {
	if (tokens.size() < 2) {
		RFC2812Handler::sendResponse(461, server.clients[client_fd], "PART :Not enough parameters");
		return;
	}
	if (!server.getClient(client_fd).isAuthenticated()) {
		RFC2812Handler::sendResponse(451, server.clients[client_fd], ":You have not registered");
		return;
	}
	std::string nickname = server.getClient(client_fd).getNickname();
	std::string message = "";
	if (tokens.size() > 2 && tokens[2] != ":")
		message = " " + tokens[2];
	PART(nickname, tokens[1], message, server);
}

void PART(std::string nickname, std::string channel, std::string message, Server& server) {
	if (server.getFdFromNickname(nickname) == -1) {
		RFC2812Handler::sendResponse(401, server.getClient(server.getFdFromNickname(nickname)), nickname + " :No such nick/channel");
		return;
	}
	if (server.getChannel(channel) == NULL) {
		RFC2812Handler::sendResponse(403, server.getClient(server.getFdFromNickname(nickname)), channel + " :No such channel");
		return;
	}
	if (!server.getChannel(channel)->isInChannel(nickname)) {
		RFC2812Handler::sendResponse(442, server.getClient(server.getFdFromNickname(nickname)), channel + " :You're not on that channel");
		return;
	}
	server.getChannel(channel)->broadcastMessageToChannel(":" + nickname + " PART " + channel + message + "\r\n", server, "");
	server.leaveChannel(channel, nickname);
}
