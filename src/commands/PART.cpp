#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <arpa/inet.h>

void CommandHandler::handlePART(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
	Channel *channel;
	Client& client = server.clients[client_fd];

	if (tokens.size() < 2)
	{
		RFC2812Handler::sendResponse(461, server.clients[client_fd], "PART :Not enough parameters");
		return;
	}
	channel = preChecks(tokens[1], client_fd, server, false);
	if (channel == NULL)
		return;
	std::string nickname = client.getNickname();
	std::string message = "";
	if (tokens.size() > 2)
		for (size_t i = 2; i < tokens.size(); i++)
			message += " " + tokens[i];
	channel->broadcastMessageToChannel(":" + nickname + "!" + client.getUsername() + "@" + inet_ntoa(client.getAddress().sin_addr) + " PART " + channel->getName() + message + "\r\n", server, "");
	server.leaveChannel(channel->getName(), nickname);
}
