#include "Command.hpp"
#include "RFC2812Handler.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <arpa/inet.h>

void CommandHandler::handleQUIT(const std::vector<std::string> &tokens, int client_fd, Server &server)
{
	Client &client = server.getClient(client_fd);
	if (!client.isAuthenticated())
	{
		server.markClientForDisconnection(client_fd);
		client.setClientQuit(true);
		return;
	}
	std::string nickname = client.getNickname();
	std::string message = ":" + nickname + "!" + client.getUsername() + "@" + inet_ntoa(client.getAddress().sin_addr) + " QUIT";
	for (size_t i = 1; i < tokens.size(); ++i)
	{
		message += " " + tokens[i];
	}
	message += "\r\n";
	std::set<std::string> commonUsers = server.getCommonUsers(client.getNickname());
	bool isBroadcasted = false;

	for (std::set<std::string>::iterator it = commonUsers.begin(); it != commonUsers.end(); ++it)
	{
		std::string nickname = *it;
		int fd = server.getFdFromNickname(nickname);
		if (fd != -1)
		{ // Check if fd is valid
			send(fd, message.c_str(), message.length(), 0);
			std::cout << LIGHT GRAY << "Sending message " << RESET << message << LIGHT GRAY << " to " << client.getNickname() << " on fd " << client_fd << RESET << std::endl;
			isBroadcasted = true;
		}
	}

	if (client.isAuthenticated() && !isBroadcasted)
	{
		if (client_fd != -1)
		{ // Check if fd is valid
			std::cout << LIGHT GRAY << "Sending message " << RESET << message << LIGHT GRAY << " to " << client.getNickname() << " on fd " << client_fd << RESET << std::endl;
			send(client_fd, message.c_str(), message.length(), 0);
		}
	}

	std::map<std::string, Channel>::iterator it;

	if (server.channels.size() > 0){
		for (it = server.channels.begin();
		it != server.channels.end();
		++it)
		{
			if (it->second.isInChannel(client.getNickname()))
			{
				server.leaveChannel(it->second.getName(), client.getNickname());
			}
			if (server.channels.size() == 0)
				break;
		}
	}

	client.setClientQuit(true);
	server.markClientForDisconnection(client_fd);
}