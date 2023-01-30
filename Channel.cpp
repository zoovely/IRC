#include "Channel.hpp"
#include <algorithm>
#include <iostream>

Channel::Channel(Client client, std::string name)
:_name(name)
{
    _users.insert(std::pair<const Client &, int> (client, OPER));
    return ;
    // 초기화 리스트로 map을 초기화해줘야함(client &라서) 괜찮은가?
}

const std::string Channel::getName(void) const {
    return _name;
}

bool Channel::checkAuth(const Client& client) const {
    if (_users.find(client) != _users.end()) {
        if (_users.find(client)->second == OPER)
            return (true);
        else
            return (false);
    }
	return (false);
}

bool Channel::checkClient(std::string nick) {
    _it = _users.begin();
    for (_it; _it != _users.end(); _it++) {
        if (_it->first.getNick() == nick)
            return (true);
    }
    return (false);
}

void Channel::addUser(const Client &client)
{
    _users.insert(std::pair<const Client&, int>(client, NORMAL));
    return ;
}

void Channel::delUser(const Client &client) {
    _users.erase(_users.find(client));
    return ;
}

void Channel::delByNick(std::string nick) {
    _it = _users.begin();
    for (_it; _it != _users.end(); _it++)
    {
        if (_it->first.getNick() == nick)
            delUser(_it->first);
    }
    return ;
}

int Channel::getUserSize( void ) const {
    return _users.size();
}

std::vector<int> Channel::getFds(int senderFd) {
    std::vector <int> fds;

    _it = _users.begin();
    for (_it; _it != _users.end(); _it++)
    {
        if (_it->first.getFd() == senderFd)
            continue;
        fds.push_back(_it->first.getFd());
    }
    return (fds);
}

void Channel::opUser(std::string nick){
    _it = _users.begin();
    for (_it; _it != _users.end(); _it++) {
        if (_it->first.getNick() == nick)
        {
            // 현재 second == oper인지확인해야하나?
            _it->second == OPER;
            return ;
        }
    }
    return ;
}

void Channel::deopUser(std::string nick){
    _it = _users.begin();
    for (_it; _it != _users.end(); _it++) {
        if (_it->first.getNick() == nick)
        {
            // 현재 second == normal인지확인해야하나?
            _it->second == NORMAL;
            return ;
        }
    }
    return ;
}