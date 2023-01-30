#include "Client.hpp"

Client::Client(std::string nick, std::string user, std::string ip, int fd)
: _nick(nick), _user(user), _ip(ip), _clientFd(fd) 
{}

const std::string Client::getNick() const{
	return (_nick);
}

const std::string Client::getUser() {
	return (_user);
}

const int Client::getFd() const {
	return (_clientFd);
}
