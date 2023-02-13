#include "Client.hpp"

Client::Client(std::string nick, std::string user, std::string ip, int fd, int flag)
: _nick(nick), _user(user), _ip(ip), _clientFd(fd), _flag(flag)
{}

void Client::setNick(std::string nick) {
	_nick = nick;
}

const std::string Client::getNick() const {
	return (_nick);
}

const std::string Client::getUser() const {
	return (_user);
}

const std::string Client::getIp() const {
	return (_ip);
}

int Client::getFd() const {
	return (_clientFd);
}

int Client::getFlag() const {
	return (_flag);
}

void Client::setFlag(int flag) {
	_flag = flag;
}

void Client::setIp(std::string ip) {
	_ip = ip;
}

void Client::setUser(std::string user) {
	_user = user;
}

bool Client::operator<(const Client& c) const {
	return (c.getNick() > this->getNick());
}