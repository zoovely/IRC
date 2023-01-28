#include "Channel.hpp"
#include <algorithm>
#include <iostream>

Channel::Channel(Client c1, Client c2)
:_name(""), _mode(PRIVATE)
{
    //user list 에 둘을 넣어줘야함.
	_users.push_back(std::make_pair(c1, NORMAL));
	_users.push_back(std::make_pair(c2, NORMAL));
    //_users[i].first, second로 확인가넝
}

Channel::Channel(std::string name)
:_name(name), _mode(PUBLIC)
{
    //맨처음 채널을 생성한 client를 user에 넣어주기
    // + operator권한 부여
}

std::string Channel::getName(void) {
    return _name;
}

bool Channel::checkAuth(Client client) {
	std::vector<std::pair<Client, int> >::iterator begin = _users.begin();
	std::vector<std::pair<Client, int> >::iterator end = _users.end();
    for (;begin != end; begin++)
    {
        if (begin->first.getNick() == client.getNick())
        {
            if (begin->second == OPER)
                return (true);
            else
                return (false);
        } 
    }
	return (false);
}

int Channel::checkClient(std::string nickName)
{
    for (int i = 0; i != _users.size(); i++)
    {
        if (_users[i].first.getNick() == nickName)
            return (i);
    }
    return (-1);
}

bool Channel::checkBan(std::string nickName)
{
    for (int i = 0; i != _ban.size(); i++)
    {
        if (_ban[i].getNick() == nickName)
            return (true);
    }
    return (false);
}

int Channel::join(Client client)
{
	if (checkBan(client.getNick()))
		return (-1); // ban 리스트에 있으면 조인 거부
	_users.push_back(std::make_pair(client, NORMAL));
	return (1);
}

int Channel::ban(Client client, std::string nickName)
{
    if (checkAuth(client) == false)
    {
        std::cout << "ban failed because " << client.getNick() << "doesn't have operator\n";
        return (-1);
    }
    int index = checkClient(nickName);
    if (index == -1)
    {
        std::cout << "ban failed because " << nickName << "doesn't exist\n";
        return (-1);
    }
    _ban.push_back(_users[index].first);
    // ban 되었다는 메세지 출력
    return (0);
}

int Channel::unban(Client client, std::string nickName)
{
    if (!checkAuth(client))
    {
        std::cout << "ban failed because" << client.getNick() << "doesn't have operator \n";
		return (-1);
    }
    for (int i = 0; i < _ban.size(); i++)
    {
        if (_ban[i].getNick() == nickName)
        {
            _ban.erase(i + _ban.begin());
            std::cout << nickName << "unbanned\n";
            // unban 되었다는 메세지 출력
            return (0);
        }
    }
    return (-2);
}

int Channel::invite(Client client, std::string nickName)
{
	if (!checkAuth(client))
		return (1); //ERR_CHANOPRIVSNEEDED
    int i = checkClient(nickName); //인자는 string입니다
    if (i > 0)
        return (2) ; // ERR_USERONCHANNEL
    return (0);
}

int Channel::kick(Client client, std::string nickName)
{
    int clientInx = checkClient(nickName);
    if (!checkAuth(client) || clientInx == -1)
		return (-1); // 불가능 메시지 보내주기
    // 권한이 있으면 nickName을 기반으로 channel에 속한 user서치
    // 해당 이름을 가진 client가 없으면 2 리턴
    std::cout << client.getNick() << " kick " << nickName << " success\n";
    // client가 있으면
    // 강제 퇴장 메세지를 channel에 전송 
    _users.erase(_users.begin() + clientInx);
    // channel user list에서 client 삭제
    return (1);
}

int Channel::op(Client client, std::string nickName)
{
	if (!checkAuth(client))
		return (-1);
	int target = checkClient(nickName);
	if (target == -1)
		return (-1);
	// 기존 권한 확인해야하는지 확인 필요
	_users[target].second = OPER;
	std::cout << "mode change perfect!!" << std::endl;
    return (1); 
}

int Channel::deop(Client client, std::string nickName)
{
    if (!checkAuth(client))
		return (-1);
	int target = checkClient(nickName);
	if (target == -1)
		return (-1);
	// 기존 권한 확인해야하는지 확인 필요
	_users[target].second = NORMAL;
	std::cout << "mode change perfect!!" << std::endl;
    return (1);
}

