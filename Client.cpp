#include "Client.hpp"

Client::Client(std::string nick, std::string user, int fd)
: _nick(nick), _user(user), _clientFd(fd) // ip 우째 : 일단은 버려
{
	
}

std::string Client::getNick() {
	return _nick;
}
