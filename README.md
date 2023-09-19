# Development TO-DO List for IRC

## Legend
- 📝 - To be done
- 🔨 - In progress
- ✅ - Done
- 🐛 - Bugged
- 🤝 - Assigned to

## Connect with irssi client
- /CONNECT -nocap -! IP port password nickname

## Initialization Phase
- [✅] Set up the project structure with a Makefile, main file, and required headers. 
- [✅] Establish basic server functionality: opening a socket, binding, and listening.

## Connection Phase
- [✅] Implement client connection and authentication.
- [✅] Store connected clients in a data structure for easy management.

## Communication Phase
- [✅] Implement basic message sending and receiving.
- [✅] Forward messages to all clients in a channel.

## Command Phase
- [✅] Implement the required commands: nickname, username, join, private message, etc. 🤝 Louis
- [📝] Implement channel operator commands: KICK, INVITE, TOPIC, MODE. 🤝 Louis

## Concurrency Phase
- [✅] Ensure that the server can handle multiple clients simultaneously. 
- [✅] Implement non-blocking I/O using poll() or its equivalent. (I decided to use select() but could be changed if needed later)

## Testing and Debugging Phase
- [✅] Choose a reference IRC client and test all functionalities. (irssi here)
- [📝] Ensure error-free operations and compliance with requirements. 
- [📝] Address any bugs and ensure stability. 

## Optimization and Finalization Phase
- [📝] Refactor code for clarity, efficiency, and organization.
- [📝] Final testing and adjustments.
