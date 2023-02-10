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

// return int값으로 주고 
//command join 할 때 여기값으로 send하기
void Channel::addUser(const Client& client)
{
    if (_users.size() == SIZE)
    {
        // sendFd(client.getFd(), ERR_CHANNELISFULL(client.getNick(), _name));
        return ;
    }
    _users.insert(std::make_pair<const Client&, int>(client, NORMAL));
    return ;
}

void Channel::delUser(const Client &client) {
    _users.erase(_users.find(client));
    return ;
}

int Channel::delByNick(std::string nick) {
    for (_it = _users.begin(); _it != _users.end(); _it++)
    {
        if (_it->first.getNick() == nick) {
            delUser(_it->first);
            return (1);
        }
    }
    return (0);
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
        std::cout << "getUserNames : nick " << _it->first.getNick() << "\n";
        std::cout << "getUserNames : mode " << _it->second << "\n";
        std::cout << "getUserNames : addr " << &_it->first << "\n";
        if(_it->second == OPER)
            msg += "@";
        msg += _it->first.getNick();
        msg += " ";
    }
    return (msg);
}