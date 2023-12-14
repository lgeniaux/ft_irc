#ifndef RFC2812HANDLER_HPP
#define RFC2812HANDLER_HPP

#include <string>
#include "Client.hpp"

class RFC2812Handler
{
public:
    static void sendInitialConnectionMessages(Client &client);
    static void sendResponse(int code, Client &client, const std::string &message);
    static std::string formatMessage(const std::string &message);
};

#endif
