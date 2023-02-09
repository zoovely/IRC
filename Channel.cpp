#include "Channel.hpp"
#include <algorithm>
#include <iostream>

Channel::Channel(const Client& client, std::string name)
:_name(name)
{
    _users.insert(std::make_pair<const Client&, int>(client, OPER));
    return ;
}

const std::string Channel::getName(void) const {
    return _name;
}

bool Channel::checkAuth(const Client& client) const {
    if (_users.find(client) != _users.end()) 
    {
        if (_users.find(client)->second == OPER)
            return (true);
        else
            return (false);
    }
	return (false);
}

bool Channel::checkClient(std::string nick) {
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        if (_it->first.getNick() == nick)
            return (true);
    }
    return (false);
}

void Channel::addUser(const Client& client)
{
    _users.insert(std::make_pair<const Client&, int>(client, NORMAL));
    return ;
}

void Channel::delUser(const Client &client) {
    _users.erase(_users.find(client));
    return ;
}

void Channel::delByNick(std::string nick) {
    for (_it = _users.begin(); _it != _users.end(); _it++)
    {
        if (_it->first.getNick() == nick) {
            delUser(_it->first);
            break;
        }
    }
    return ;
}

int Channel::getUserSize( void ) const {
    return _users.size();
}

std::vector<int> Channel::getFds(int senderFd) const {
    std::vector <int> fds;
    std::map<const Client&, int>::const_iterator it;

    for (it = _users.begin(); it != _users.end(); it++)
    {
        if (it->first.getFd() == senderFd)
            continue;
        fds.push_back(it->first.getFd());
    }
    return (fds);
}

void Channel::opUser(std::string nick){
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        if (_it->first.getNick() == nick)
        {
            _it->second = OPER;
            return ;
        }
    }
    return ;
}

void Channel::deopUser(std::string nick){
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        if (_it->first.getNick() == nick)
        {
            _it->second = NORMAL;
            return ;
        }
    }
    return ;
}

std::string Channel::getUsersNames(void) {
    std::string msg;
    _it = _users.begin();
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        if(_it->second == OPER)
            msg += "@";
        msg += _it->first.getNick();
        msg += " ";
    }
    return (msg);
}