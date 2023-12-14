/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcollon <jcollon@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 23:48:50 by jcollon           #+#    #+#             */
/*   Updated: 2023/12/14 00:07:22 by jcollon          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

std::pair<Channel *, Client> CommandHandler::preChecks(const std::string nickname, int client_fd, Server &server, const bool needOp)
{
	Channel *channel;
	Client client;

	client = server.getClient(client_fd);
	if (!client.isAuthenticated())
	{
		RFC2812Handler::sendResponse(451, server.clients[client_fd], ":You have not registered");
		return std::make_pair((Channel *)NULL, client);
	}
	channel = server.getChannel(nickname);
	if (channel == NULL)
	{
		RFC2812Handler::sendResponse(403, server.clients[client_fd], nickname + " :No such channel");
		return std::make_pair((Channel *)NULL, client);
	}
	if (!channel->isInChannel(client.getNickname()))
	{
		RFC2812Handler::sendResponse(442, server.clients[client_fd], nickname + " :You're not on that channel");
		return std::make_pair((Channel *)NULL, client);
	}
	if (needOp && !channel->isOperator(client.getNickname()))
	{
		RFC2812Handler::sendResponse(482, server.clients[client_fd], nickname + " :You're not channel operator");
		return std::make_pair((Channel *)NULL, client);
	}
	return std::make_pair(channel, client);
}