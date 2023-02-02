#include "Channel.hpp"
#include <algorithm>
#include <iostream>

Channel::Channel(const Client& client, std::string name)
:_name(name)
{
    std::pair<const Client&, int> newpair(client, OPER);
    _users.insert(newpair);
    return ;
    // 초기화 리스트로 map을 초기화해줘야함(client &라서) 괜찮은가? 무슨말이지?
    // makepair로 insert 를하면 makepair 가 레퍼런스로 안받아서 데이터가 사라지는듯? 확실치않음 근데안됨 이렇게하면 됨;
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
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        if (_it->first.getNick() == nick)
            return (true);
    }
    return (false);
}

void Channel::addUser(const Client& client)
{
    std::pair<const Client&, int> newpair(client, NORMAL); // makepair 에서 수정함
    std::cout << "adduser nick :" << client.getNick() << " fd :" << client.getFd() << "\n";
    _users.insert(newpair);
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        std::cout << _it->first.getNick();
    }   
    std::cout << "\n";
    std::cout << "size :" << _users.size() << "maxsize :" << _users.max_size() << "\n";
    return ;
}

void Channel::delUser(const Client &client) {
    _users.erase(_users.find(client));
    return ;
}

void Channel::delByNick(std::string nick) {
    for (_it = _users.begin(); _it != _users.end(); _it++)
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

    for (_it = _users.begin(); _it != _users.end(); _it++)
    {
        if (_it->first.getFd() == senderFd)
            continue;
        std::cout << "nick :" << _it->first.getNick() << "push fds : " << _it->first.getFd() << "\n";
        fds.push_back(_it->first.getFd());
    }
    return (fds);
}

void Channel::opUser(std::string nick){
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        if (_it->first.getNick() == nick)
        {
            // 현재 second == oper인지확인해야하나?
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
            // 현재 second == normal인지확인해야하나?
            _it->second = NORMAL;
            return ;
        }
    }
    return ;
}

std::string Channel::getUsersNames(void) {
    std::string msg;
    _it = _users.begin(); //
    for (_it = _users.begin(); _it != _users.end(); _it++) {
        if(_it->second == OPER)
            msg += "@";
        msg += _it->first.getNick();
        msg += " ";
    }
    return (msg);
}