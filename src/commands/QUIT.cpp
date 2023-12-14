#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"
#include <unistd.h>

void CommandHandler::handleQUIT(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
	Client &client = server.getClient(client_fd);
	if (!client.isAuthenticated())
	{
		server.markClientForDisconnection(client_fd);
		return;
	}
	std::string message = ":" + client.getNickname() + " QUIT";
	for (size_t i = 1; i < tokens.size(); ++i)
	{
		message += " " + tokens[i];
	}
	message += "\r\n";
	std::map<std::string, Channel>::iterator it;
	for (it = server.channels.begin(); it != server.channels.end(); ++it)
	{
		if (it->second.isInChannel(client.getNickname()))
		{
			it->second.broadcastMessageToChannel(message, server, "");
			server.leaveChannel(it->second.getName(), client.getNickname());
		}
	}
	server.markClientForDisconnection(client_fd);
}