#ifndef COLORS_AND_ASCII_HPP
#define COLORS_AND_ASCII_HPP

#define BOLD "\033[1m"
#define LIGHT "\033[2m"
#define GRAY "\033[90m"
#define RED "\033[31m"
#define PURPLE "\033[35m"
#define CYAN "\033[36m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

#define ERROR BOLD RED "[Error] " RESET
#define WARN BOLD PURPLE "[Warning] " RESET
#define INFO BOLD CYAN "[Info] " RESET

#define WELCOME_MSG GREEN "Welcome to the Internet Relay Network ${NAME}! " LIGHT GRAY "${USER}@${HOST}" RESET

#endif // COLORS_AND_ASCII_HPP