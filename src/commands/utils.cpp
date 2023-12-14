/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcollon <jcollon@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/13 23:48:50 by jcollon           #+#    #+#             */
/*   Updated: 2023/12/14 22:34:25 by jcollon          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

Channel* CommandHandler::preChecks(const std::string channelName, int client_fd, Server &server, const bool needOp)
{
	Channel *channel;
	Client& client = server.getClient(client_fd);

	std::cout << client.isAuthenticated() << std::endl;
	if (!client.isAuthenticated())
	{
		RFC2812Handler::sendResponse(451, server.clients[client_fd], ":You have not registered");
		return (NULL);
	}
	channel = server.getChannel(channelName);
	if (channel == NULL)
	{
		RFC2812Handler::sendResponse(403, server.clients[client_fd], channelName + " :No such channel");
		return (NULL);
	}
	if (!channel->isInChannel(client.getNickname()))
	{
		RFC2812Handler::sendResponse(442, server.clients[client_fd], channelName + " :You're not on that channel");
		return (NULL);
	}
	if (needOp && !channel->isOperator(client.getNickname()))
	{
		RFC2812Handler::sendResponse(482, server.clients[client_fd], channelName + " :You're not channel operator");
		return (NULL);
	}
	return (channel);
}