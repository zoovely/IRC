#include "Command.hpp"
#include <iostream>
#include <algorithm>

Command::Command(std::string msg) {
	_msg = msg;
	splitMsg();
}

void Command::splitMsg(void) {
	std::string::size_type pos = _msg.find(" " , 0);
	std::string::size_type newpos = _msg.find("\r\n", 0);
	std::string::size_type colone = _msg.find(":", 0);

	int i = 0;
	while ((pos != std::string::npos || newpos != std::string::npos) && pos < colone)
	{
		if (pos > newpos)
		{
			_splitMsg.push_back(_msg.substr(i, newpos - i));
			i = newpos + 2;
		}
		else {
			_splitMsg.push_back(_msg.substr(i, pos - i));
			i = pos + 1;
		}
		pos = _msg.find(" ", i);
		newpos = _msg.find("\r\n", i);
	}
	if (colone != std::string::npos)
		_splitMsg.push_back(_msg.substr(colone, _msg.length() - colone -2));
	else
		_splitMsg.push_back(_msg.substr(i, _msg.length() - i - 2));
	return ;
}

int	Command::checkMsgType(void) {
	std::string typeList[] = {"CAP", "PASS", "JOIN", "PART", "INVITE", "KICK", "NICK", "USER", "LIST", "WHOIS", "QUIT", "PING", "MODE", "PRIVMSG", "NOTICE"};
	
	for (size_t i = 0; i < sizeof(typeList)/sizeof(std::string); i++) {
		if (_splitMsg[0].find(typeList[i]) != std::string::npos) {
			switch (i)
			{
				case 12:
					if (_splitMsg[2].find("+o", 0) != std::string::npos)
						return (OP);
					else if (_splitMsg[2].find("-o", 0) != std::string::npos)
						return (DEOP);
					else if (_splitMsg[2].find("+i", 0) != std::string::npos)
						return (MODE_I);
					else if (_splitMsg[1].find("#", 0) != std::string::npos)
						return (MODE_N);
					break;
				case 13:
					if (_splitMsg[1].find("#", 0) == std::string::npos)
						return (PRIVMSG);
					else
						return (PRIVCH);
				case 14:
					if (_splitMsg[1].find("#", 0) == std::string::npos)
						return (NOTICE);
					else
						return (NOTICH);
				default:
					return (i + 1);
			}
		}
	}
	return (0);
}

void Command::sendFd(int fd, std::string str) {
	int ret = send(fd, str.c_str(), str.size(), MSG_DONTWAIT);
	if (ret == -1) {
		std::cerr << str.c_str() << "\n";
	}
	return ;
}

void Command::sendAll(std::vector<int> fds, std::string str) {
	for (size_t i = 0; i < fds.size(); i++)
		sendFd(fds[i], str);
	return ;
}

//channel 이름으로 list에서 찾는 함수
int Command::checkValidChannel(const std::string chName, const std::vector<Channel> &chList) {
	for (size_t i = 0; i < chList.size(); i++) {
		if (chList[i].getName() == chName) {
			return (i);
		}
	}
	return (-1);
}

//nick네임으로 client list에서 찾는 함수
int Command::checkValidClient(const std::string nick, const std::vector<Client> &cList) {
	for (size_t i = 0; i < cList.size(); i++) {
		if (cList[i].getNick() == nick) {
			return (i);
		}
	}
	return (-1);
}

int	Command::checkValidNick(const std::string nick) {
	std::string	cmdList[] = {"CAP", "PASS", "JOIN", "PART", "INVITE", "KICK", "NICK", "USER", "LIST", "WHOIS", "QUIT", "PING", "MODE", "PRIVMSG", "NOTICE"};
	for (size_t i = 0; i < cmdList->size(); i++) {
		if (nick == cmdList[i])
			return (-1);
	}
	for (size_t i = 0; i < nick.size(); i++) {
		if ((!std::isalnum(nick[i]) && nick[i] != '-' && nick[i] != '_'))
			return (-1);
	}
	return (1);
}

int Command::getClientByFd(int fd, std::vector<Client> cList) {
	for(size_t i = 0; i < cList.size(); i++) {
		if (cList[i].getFd() == fd)
			return (i);
	}
	return (-1);
}

//command list 
int Command::connect(int fd, std::string pwd, std::vector<Client> &cList) {	
	Client nClient("", "", "", fd, A_CONNECT);
	cList.push_back(nClient);
	if (_splitMsg.size() != 2 && _splitMsg[2] == "PASS") {
		pass(cList.back(), pwd, cList, cList.size() - 1);
	}
	return (1);
}

int Command::pass(Client &client, std::string pwd, std::vector<Client> &cList, int idx) {
	size_t i;

	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("PASS") == 0)
			break;
	}
	if (i == _splitMsg.size() || _splitMsg[i + 1].compare(pwd) != 0) {
		sendFd(client.getFd(), ERR_PASSWDMISMATCH);
		cList.erase(idx + cList.begin());
		return (-1);
	}
	client.setFlag(A_PASS);
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("NICK") == 0)
		{
			std::vector<Channel> chList;
			nick(client, cList, chList);
			break;
		}
	}
	return (1);
}


int Command::join(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	size_t i;
	for (i = 0; i < chList.size(); i++) {
		if (chList[i].getName() == chName) {
			chList[i].addUser(client);
			std::vector<int> fds = chList[i].getFds(client.getFd());
			sendAll(fds, RPL_JOIN(client.getNick(), client.getIp(), chName));
			break ;
		}
	}
	if (i == chList.size())
	{
		Channel ch(client, chName);
		chList.push_back(ch);
	}
	sendFd(client.getFd(), RPL_JOIN(client.getNick(), client.getIp(), chName));
	sendFd(client.getFd(), RPL_NAMREPLY(client.getNick(), chName, chList[i].getUsersNames()));
	sendFd(client.getFd(), RPL_ENDOFNAMES(client.getNick(), chName));
	return (1);
}

void Command::delChannel(std::vector<Channel> &chList, Channel &channel) {
	if (chList.size() == 1)
	{
		chList.clear();
		return ;
	}
	std::vector<Channel>::iterator it;
	for (it = chList.begin(); it != chList.end(); it++)
	{
		if ((*it).getName() == channel.getName())
			chList.erase(it);
	}
	return ;
}

// part #chName msg?
int Command::part(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1) {
		sendFd(client.getFd(), ERR_NOSUCHCHANNEL(client.getNick(), chName));
		return (-1);
	}
	Channel &ch = chList[chIdx];
	if (!ch.checkClient(client.getNick())) {
		sendFd(client.getFd(), ERR_NOTONCHANNEL(client.getNick(), chName));
		return (-1);
	}
	ch.delUser(client);
	sendFd(client.getFd(), RPL_PART(client.getNick(), client.getIp(), chName));
	if (ch.getUserSize() == 0) {
		delChannel(chList, chList[chIdx]);
		return (1);
	}
	sendAll(ch.getFds(client.getFd()), RPL_PART(client.getNick(), client.getIp(), chName));
	return (1);
}

// invite nick chName
int Command::invite(const Client &client, const std::vector<Channel> &chList, const std::vector<Client> &cList) {
	std::string target = _splitMsg[1];
	std::string chName = _splitMsg[2];

	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1) {
		sendFd(client.getFd(), ERR_NOSUCHCHANNEL(client.getNick(), chName));
		return (-1);
	}
	if (!chList[chIdx].checkAuth(client)) {
		sendFd(client.getFd(), ERR_CHANOPRIVSNEEDED(client.getNick(), chName));
		return (-1);
	}
	int targetIdx = checkValidClient(_splitMsg[1], cList);
	if (targetIdx == -1) {
		sendFd(client.getFd(), ERR_NOSUCHNICK(client.getNick(), target));
		return (-1);
	}
	sendFd(client.getFd(), RPL_INVITING(client.getNick(), target, chName));
	sendFd(cList[targetIdx].getFd(), RPL_INVITED(client.getNick(), chName)); 
	return (1);
}

// kick chName nick
int Command::kick(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	std::string target = _splitMsg[2];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1) {
		sendFd(client.getFd(), ERR_NOSUCHCHANNEL(client.getNick(), chName));
		return (-1);
	}
	Channel &channel = chList[chIdx];
	if (!chList[chIdx].checkAuth(client)) {
		sendFd(client.getFd(), ERR_CHANOPRIVSNEEDED(client.getNick(), chName));
		return (-1);
	}
	if (!channel.checkClient(target)) {
		sendFd(client.getFd(), ERR_USERNOTINCHANNEL(client.getNick(), target, chName));
		return (-1);
	}
	std::string msg = "";
	if (_splitMsg.size() > 3)
		msg = _splitMsg[3];
	sendFd(client.getFd(), RPL_KICK(client.getNick(), client.getNick(), client.getIp(), chName, target, msg));
	sendAll(channel.getFds(client.getFd()), RPL_KICK(client.getNick(), client.getNick(), client.getIp(), chName, target, msg));
	channel.delByNick(target);
	if (channel.getUserSize() == 0)
		delChannel(chList, channel);
    return (1);
}

int Command::op(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	std::string nick = _splitMsg[3];
	int cFd = client.getFd();
	std::string msg;
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1)
	{
		msg = ERR_NOSUCHNICK(client.getNick(), chName);
		sendFd(cFd, msg);
		return (-1);
	}
	Channel &channel = chList[chIdx];
	if (channel.checkAuth(client) == false)
	{
		msg = ERR_CHANOPRIVSNEEDED(client.getNick(), chName);
		sendFd(cFd, msg);	
		return (-1);
	}
	if (channel.checkClient(nick) == false)
	{
		msg = ERR_USERNOTINCHANNEL(client.getNick(), nick, chName);
		sendFd(cFd, msg);	
		return (-1);
	}
	else 
	{
		channel.opUser(nick);
		msg = RPL_OP(client.getNick(), client.getNick(), client.getIp(), chName, nick);
		sendAll(channel.getFds(cFd), msg);
	}
	sendFd(cFd, msg);
	return (1);
}

int Command::deop(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	std::string nick = _splitMsg[3];
	int cFd = client.getFd();
	std::string msg;
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1)
	{
		msg = ERR_NOSUCHNICK(client.getNick(), chName);
		sendFd(cFd, msg);
		return (-1);
	}
	Channel &channel = chList[chIdx];
	if (channel.checkAuth(client) == false)
	{
		msg = ERR_CHANOPRIVSNEEDED(client.getNick(), chName);
		sendFd(cFd, msg);
		return (-1); 
	}
	if (channel.checkClient(nick) == false)
	{
		msg = ERR_USERNOTINCHANNEL(client.getNick(), nick, chName);
		sendFd(cFd, msg);
		return (-1);
	}
	else
	{
		channel.deopUser(nick);
		msg = RPL_DEOP(client.getNick(), client.getNick(), client.getIp(), chName, nick);
		sendAll(channel.getFds(cFd), msg);
	}
	sendFd(cFd, msg);
	return (1);
}

// nick change nick
int Command::nick(Client &client, std::vector<Client> &cList, const std::vector<Channel> &chList) {
	std::vector<int>	mList;
	std::vector<int>	temp;
	std::vector<const Channel>::const_iterator it;
	std::string 		msg;
	std::string 		nickName;
	int cFd = client.getFd();

	if (client.getFlag() < A_PASS) {
		sendFd(cFd, ERR_PASSWDMISMATCH);
		cList.erase(getClientByFd(cFd, cList) + cList.begin());
		return (-1);
	}
	if (client.getFlag() == A_PASS)
	{
		size_t i;
		for (i = 0; i < _splitMsg.size(); i++) {
			if (_splitMsg[i].compare("NICK") == 0) {
				nickName = _splitMsg[i + 1];
				break;
			}
		}
		if (chkNick(nickName, cList, cFd) == -1)
		{
			if (checkValidNick(nickName) == -1)
				return (-1);
			cList.erase(getClientByFd(cFd, cList) + cList.begin());
			return (-1);
		}
		client.setNick(nickName);
		client.setFlag(A_NICK);
		if (_splitMsg.size() > i + 2 && _splitMsg[i + 2].compare("USER") == 0)
			user(client, cList);
	}
	else {
 		nickName = _splitMsg[1];
		if (chkNick(nickName, cList, cFd) == -1)
			return (-1);
		for (it = chList.begin(); it < chList.end(); it++) {
			temp = it->getFds(client.getFd());
			mList.insert(mList.end(), temp.begin(), temp.end());
		}
		sort(mList.begin(), mList.end());
		mList.erase(unique(mList.begin(), mList.end()), mList.end());
		msg = RPL_NICK(client.getNick(), client.getNick(), client.getIp(), nickName);
		sendFd(cFd, msg);
		sendAll(mList, msg);
		client.setNick(nickName);
	}
	return (1);
}

int Command::chkNick(std::string nickName, std::vector<Client> &cList, int cFd) {
	if (nickName.empty() == true)
	{
		sendFd(cFd, ERR_NONICKNAMEGIVEN);
		return (-1);
	}
	if (checkValidClient(nickName, cList) != -1)
	{
		sendFd(cFd, ERR_NICKNAMEINUSE(nickName));
		return (-1);
	}
	if (checkValidNick(nickName) == -1)
	{	
		sendFd(cFd, ERR_ERRONEUSNICKNAME(nickName));
		return (-1);
	}
	return (1);
}

int Command::user(Client &client, std::vector<Client> &cList) {
	size_t i;

	if (client.getFlag() != A_NICK) {
		cList.erase(getClientByFd(client.getFd(), cList) + cList.begin());
		return (-1);
	}
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("USER") == 0)
			break;
	}
	if (i == _splitMsg.size()) {
		cList.erase(getClientByFd(client.getFd(), cList) + cList.begin());
		return (-1);
	}
	client.setUser(_splitMsg[i + 4].erase(0, 1));
	client.setIp(_splitMsg[i + 3]);
	client.setFlag(A_USER);

	sendFd(client.getFd(), RPL_WELCOME(client.getNick()));
	sendFd(client.getFd(), RPL_YOURHOST(client.getNick()));
	return (1);
}

// list
int Command::list(const Client &client, const std::vector<Channel> &chList) {
	int cFd = client.getFd();
	std::string msg;
	msg = RPL_LISTSTART(client.getNick());
	sendFd(cFd, msg);
	std::vector<Channel>::const_iterator coit;
	for (coit = chList.begin(); coit != chList.end(); coit++)
	{
		msg = RPL_LIST(client.getNick(), coit->getName(), std::to_string(coit->getUserSize()));
		sendFd(cFd, msg);
	}
	msg = RPL_LISTEND(client.getNick());
	sendFd(cFd, msg);
	return (1);
}

// whois nick
int Command::whois(const Client &client, const std::vector<Client> &cList) {
	int cFd = client.getFd();
	std::string target = _splitMsg[1];
	int cIdx = checkValidClient(target, cList);
	if (cIdx == -1) {
		sendFd(cFd, ERR_NOSUCHNICK(client.getNick(), target));
		return (-1);
	}

	Client temp = cList[cIdx];
	sendFd(cFd, RPL_WHOISUSER(temp.getNick(), temp.getUser(), temp.getIp()));
	sendFd(cFd, RPL_WHOISSERVER);
	sendFd(cFd, RPL_WHOISMODE(temp.getNick()));
	sendFd(cFd, RPL_ENDOFWHOIS);
	return (1);
}

int Command::quit(Client &client, std::vector<Channel> &chList, std::vector<Client> &cList) {
	std::vector<int>	mList;
	std::vector<int>	temp;
	
	for (size_t i = 0; i < chList.size(); i++) {
		chList[i].delByNick(client.getNick());
		temp = chList[i].getFds(client.getFd());
		mList.insert(mList.end(), temp.begin(), temp.end());
		if (chList[i].getUserSize() == 0)
			chList.erase(chList.begin() + i);
	}
	sort(mList.begin(), mList.end());
	mList.erase(unique(mList.begin(), mList.end()), mList.end());
	std::string msg = "";
	if (_splitMsg.size() == 2)
		msg = _splitMsg[1];
	sendAll(mList, RPL_QUIT(client.getNick(), client.getNick(), client.getIp(), msg));
	cList.erase(checkValidClient(client.getNick(), cList) + cList.begin());
	return (1);
} 

int Command::ping(const Client &client) {
	sendFd(client.getFd(), RPL_PONG(_splitMsg[1]));
	return (1);
}

// privMsg #chName msg
int Command::privmsg(const Client &sender, const std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	int chIdx = checkValidChannel(_splitMsg[1], chList);
	if (chIdx == -1) {
		sendFd(sender.getFd(), ERR_NOSUCHCHANNEL(sender.getNick(), chName));
		return (-1);
	}
	Channel channel = chList[chIdx];
	if (!channel.checkClient(sender.getNick())) {
		sendFd(sender.getFd(), ERR_NOTONCHANNEL(sender.getNick(), chName));
		return (-1);
	}
	std::vector<int> fds = channel.getFds(sender.getFd()); 
  	sendAll(channel.getFds(sender.getFd()),
    RPL_PRIVMSG(sender.getNick(), sender.getNick(), sender.getIp(), chName, _splitMsg[2]));
	return (1);
}

// privmsg nick msg //
int Command::privmsg(const Client &sender, const std::vector<Client> &cList) {
	std::string cName = _splitMsg[1];
	int cIdx = checkValidClient(cName, cList);
	if (cIdx == -1) {
		sendFd(sender.getFd(), ERR_NOSUCHNICK(sender.getNick(), cName));
		return (-1);
	}
	sendFd(sender.getFd(), RPL_PRIVMSG(sender.getNick(), sender.getNick(), sender.getIp(), cName, _splitMsg[2]));
	return (1);
}

int Command::notice(const Client &sender, const std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1) {
		sendFd(sender.getFd(), ERR_NOSUCHCHANNEL(sender.getNick(), chName));
		return (-1);
	}
	Channel channel = chList[chIdx];
	if (channel.checkClient(sender.getNick()) == false) {
		sendFd(sender.getFd(), ERR_NOTONCHANNEL(sender.getNick(), chName));
		return (-1);
	}
	std::vector<int> fds = channel.getFds(sender.getFd());  
	sendAll(fds, RPL_NOTICE(sender.getNick(), sender.getNick(), sender.getIp(), chName, _splitMsg[2]));
	return (1);
}

int Command::notice(const Client &sender, const std::vector<Client> &cList) {
	std::string cName = _splitMsg[1];
	int cIdx = checkValidClient(cName, cList);
	if (cIdx == -1) {
		sendFd(sender.getFd(), ERR_NOSUCHNICK(sender.getNick(), cName));
		return (-1);
	}
	sendFd(cList[cIdx].getFd(), RPL_NOTICE(sender.getNick(), sender.getNick(), sender.getIp(), cName, _splitMsg[2]));
	return (1);
}

int Command::modeI(const Client &sender) {
	sendFd(sender.getFd(), RPL_MODE_I(sender.getNick(), sender.getIp()));
	return (1);
}

int Command::modeN(const Client &sender , const std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	int cFd = sender.getFd();
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1)
	{
		sendFd(cFd, ERR_NOSUCHNICK(sender.getNick(), chName));
		return (-1);
	}
	sendFd(cFd, RPL_MODE_N(sender.getNick(), chName));
	return (1);
}