#include "Command.hpp"
#include <iostream>
#include <algorithm>
#include <list>

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
std::list<Channel>::const_iterator Command::checkValidChannel(const std::string chName, const std::list<Channel> &chList) {
	for (_coChit = chList.begin(); _coChit != chList.end(); _coChit++) {
		if (_coChit->getName() == chName)
			break ;
	}
	return (_coChit);
}

//nick네임으로 client list에서 찾는 함수
std::list<Client>::const_iterator Command::checkValidClient(const std::string nick, const std::list<Client> &cList) {
	for (_coCit = cList.begin(); _coCit != cList.end(); _coCit++) {
		if (_coCit->getNick() == nick)
			break;
	}
	return (_coCit);
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

std::list<Client>::iterator Command::getClientByFd(int fd, std::list<Client> cList) {
	for (_cit = cList.begin(); _cit != cList.end(); _cit++) {
		if (_cit->getFd() == fd)
			break;
	}
	return (_cit);
}

void Command::welcomeMsg(Client &client)
{
	sendFd(client.getFd(), RPL_WELCOME(client.getNick()));
	sendFd(client.getFd(), RPL_YOURHOST(client.getNick()));
	client.setFlag(DONE);
	return ;
}

//command list 
int Command::connect(int fd, std::string pwd, std::list<Client> &cList) {	
	Client nClient("", "", "", fd, A_CONNECT);
	cList.push_back(nClient);
	if (_splitMsg.size() != 2 && _splitMsg[2] == "PASS") {
		pass(cList.back(), pwd, cList, getClientByFd(fd, cList));
	}
	return (1);
}

int Command::pass(Client &client, std::string pwd, std::list<Client> &cList, std::list<Client>::iterator cIt) {
	size_t i;

	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("PASS") == 0)
			break;
	}
	if (i == _splitMsg.size() || _splitMsg[i + 1].compare(pwd) != 0) {
		sendFd(client.getFd(), ERR_PASSWDMISMATCH);
		cList.erase(cIt);
		return (-1);
	}
	client.setFlag(A_PASS);
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("NICK") == 0)
		{
			std::list<Channel> chList;
			nick(client, cList, chList);
			break;
		}
	}
	return (1);
}

int Command::join(const Client &client, std::list<Channel> &chList) {
	if (_splitMsg.size() < 2)
		return (-1);
	std::string chName = _splitMsg[1];
	std::string userNames = "";
	for (_chit = chList.begin(); _chit != chList.end(); _chit++) {
		if (_chit->getName() == chName) {
			if (_chit->getUserSize() == SIZE)
			{
				sendFd(client.getFd(), ERR_CHANNELISFULL(client.getNick(), chName));
				return (-1);
			}
			_chit->addUser(client);
			std::vector<int> fds = _chit->getFds(client.getFd());
			sendAll(fds, RPL_JOIN(client.getNick(), client.getIp(), chName));
			userNames = _chit->getUsersNames();
			break ;
		}
	}
	if (_chit == chList.end())
	{
		Channel ch(client, chName);
		chList.push_back(ch);
		userNames = ch.getUsersNames();
	}
	sendFd(client.getFd(), RPL_JOIN(client.getNick(), client.getIp(), chName));
	sendFd(client.getFd(), RPL_NAMREPLY(client.getNick(), chName, userNames));
	sendFd(client.getFd(), RPL_ENDOFNAMES(client.getNick(), chName));
	return (1);
}

void Command::delChannel(std::list<Channel> &chList, std::string chName) {
	if (chList.size() == 1)
	{
		chList.clear();
		return ;
	}
	for (_chit = chList.begin(); _chit != chList.end(); _chit++)
	{
		if ((*_chit).getName() == chName)
			chList.erase(_chit);
	}
	return ;
}

// part #chName msg?
int Command::part(const Client &client, std::list<Channel> &chList) {
	if (_splitMsg.size() < 2)
		return (-1);
	
	std::string chName = _splitMsg[1];
	_coChit = checkValidChannel(chName, chList);
	if (_coChit == chList.end()) {
		sendFd(client.getFd(), ERR_NOSUCHCHANNEL(client.getNick(), chName));
		return (-1);
	}
	Channel &ch = const_cast <Channel&> (*_coChit);
	if (!ch.checkClient(client.getNick())) {
		sendFd(client.getFd(), ERR_NOTONCHANNEL(client.getNick(), chName));
		return (-1);
	}
	ch.delUser(client);
	sendFd(client.getFd(), RPL_PART(client.getNick(), client.getIp(), chName));
	if (_coChit->getUserSize() == 0) {
		delChannel(chList, _coChit->getName());
		return (1);
	}
	sendAll(_coChit->getFds(client.getFd()), RPL_PART(client.getNick(), client.getIp(), chName));
	return (1);
}

// invite nick chName
int Command::invite(const Client &client, const std::list<Channel> &chList, const std::list<Client> &cList) {
	if (_splitMsg.size() < 3)
		return (-1);
	std::string target = _splitMsg[1];
	std::string chName = _splitMsg[2];
	_coChit = checkValidChannel(chName, chList);
	if (_coChit == chList.end()) {
		sendFd(client.getFd(), ERR_NOSUCHCHANNEL(client.getNick(), chName));
		return (-1);
	}
	if (!_coChit->checkAuth(client)) {
		sendFd(client.getFd(), ERR_CHANOPRIVSNEEDED(client.getNick(), chName));
		return (-1);
	}
	_coCit = checkValidClient(_splitMsg[1], cList);
	if (_coCit == cList.end()) {
		sendFd(client.getFd(), ERR_NOSUCHNICK(client.getNick(), target));
		return (-1);
	}
	sendFd(client.getFd(), RPL_INVITING(client.getNick(), target, chName));
	sendFd(_coCit->getFd(), RPL_INVITED(client.getNick(), chName)); 
	return (1);
}

// kick chName nick
int Command::kick(const Client &client, std::list<Channel> &chList) {
	if (_splitMsg.size() < 3)
		return (-1);
	std::string chName = _splitMsg[1];
	std::string target = _splitMsg[2];
	_coChit = checkValidChannel(chName, chList);
	if (_coChit == chList.end()) {
		sendFd(client.getFd(), ERR_NOSUCHCHANNEL(client.getNick(), chName));
		return (-1);
	}
	Channel &channel = const_cast<Channel &>(*_coChit);
	if (!channel.checkAuth(client)) {
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
		delChannel(chList, _coChit->getName());
    return (1);
}

int Command::op(const Client &client, std::list<Channel> &chList) {
	if (_splitMsg.size() < 4)
		return (-1);
	std::string chName = _splitMsg[1];
	std::string nick = _splitMsg[3];
	int cFd = client.getFd();
	std::string msg;
	_coChit = checkValidChannel(chName, chList);
	if (_coChit == chList.end())
	{
		msg = ERR_NOSUCHNICK(client.getNick(), chName);
		sendFd(cFd, msg);
		return (-1);
	}
	Channel &channel = const_cast <Channel&> (*_coChit);
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

int Command::deop(const Client &client, std::list<Channel> &chList) {
	if (_splitMsg.size() < 4)
		return (-1);
	std::string chName = _splitMsg[1];
	std::string nick = _splitMsg[3];
	int cFd = client.getFd();
	std::string msg;
	_coChit = checkValidChannel(chName, chList);
	if (_coChit == chList.end())
	{
		msg = ERR_NOSUCHNICK(client.getNick(), chName);
		sendFd(cFd, msg);
		return (-1);
	}
	Channel &channel = const_cast<Channel &>(*_coChit);
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
int Command::nick(Client &client, std::list<Client> &cList, const std::list<Channel> &chList) {
	std::vector<int>	mList;
	std::vector<int>	temp;
	std::string 		msg;
	std::string 		nickName;
	int cFd = client.getFd();

	if (client.getFlag() < A_PASS) {
		sendFd(cFd, ERR_PASSWDMISMATCH);
		cList.erase(getClientByFd(cFd, cList));
		return (-1);
	}
	size_t i;
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("NICK") == 0) {
			nickName = _splitMsg[i + 1];
			break;
		}
	}
	if (client.getFlag() == DUPDUP)
	{
		std::cout << "here\n";
		if (chkNick(nickName, cList, cFd) == -1)
			return (-1);
		client.setNick(nickName);
		welcomeMsg(client);
		return (1);
	}
	if (client.getFlag() == A_PASS)
	{
		if (chkNick(nickName, cList, cFd) == -1)
		{
			if (checkValidClient(nickName, cList) != cList.end())
			{
				client.setFlag(DUPDUP);
				if (_splitMsg.size() > i + 2 && _splitMsg[i + 2].compare("USER") == 0)
					user(client, cList);
				return (-1);
			} 
			else {
				cList.erase(getClientByFd(cFd, cList));
				return (-1);
			}
		}
		client.setNick(nickName);
		client.setFlag(A_NICK);
		if (_splitMsg.size() > i + 2 && _splitMsg[i + 2].compare("USER") == 0)
			user(client, cList);
	}
	else { //1111_ 여기
 		nickName = _splitMsg[1];
		if (chkNick(nickName, cList, cFd) == -1) // 걸림
			return (-1);
		for (_coChit = chList.begin(); _coChit != chList.end(); _coChit++) {
			temp = _coChit->getFds(client.getFd());
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

int Command::chkNick(std::string nickName, std::list<Client> &cList, int cFd) {
	if (nickName.empty() == true)
	{
		sendFd(cFd, ERR_NONICKNAMEGIVEN);
		return (-1);
	}
	if (checkValidClient(nickName, cList) != cList.end())
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

int Command::user(Client &client, std::list<Client> &cList) {
	size_t i;

	if (client.getFlag() != A_NICK && client.getFlag() != DUPDUP) {
		cList.erase(getClientByFd(client.getFd(), cList));
		return (-1);
	}
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("USER") == 0)
			break;
	}
	if (i == _splitMsg.size()) {
		cList.erase(getClientByFd(client.getFd(), cList));
		return (-1);
	}
	if (_splitMsg.size() < i + 5) {
		sendFd(client.getFd(), ERR_NEEDMOREPARAMS(client.getNick(), "USER"));
		return (-1);
	}
	client.setUser(_splitMsg[i + 4].erase(0, 1));
	client.setIp(_splitMsg[i + 3]);
	if (client.getFlag() == A_NICK)
		welcomeMsg(client);
	return (1);
}

// list
int Command::list(const Client &client, const std::list<Channel> &chList) {
	int cFd = client.getFd();
	std::string msg;
	msg = RPL_LISTSTART(client.getNick());
	sendFd(cFd, msg);
	std::list<Channel>::const_iterator coit;
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
int Command::whois(const Client &client, const std::list<Client> &cList) {
	if (_splitMsg.size() < 2)
		return (-1);
	int cFd = client.getFd();
	std::string target = _splitMsg[1];
	_coCit = checkValidClient(target, cList);
	if (_coCit == cList.end()) {
		sendFd(cFd, ERR_NOSUCHNICK(client.getNick(), target));
		return (-1);
	}
	sendFd(cFd, RPL_WHOISUSER(_coCit->getNick(), _coCit->getUser(), _coCit->getIp()));
	sendFd(cFd, RPL_WHOISSERVER);
	sendFd(cFd, RPL_WHOISMODE(_coCit->getNick()));
	sendFd(cFd, RPL_ENDOFWHOIS);
	return (1);
}

int Command::quit(std::list<Client>::iterator cIt, std::list<Channel> &chList, std::list<Client> &cList) {
	std::vector<int>	mList;
	std::vector<int>	temp;
	
	for (_chit = chList.begin(); _chit != chList.end(); _chit++) {
		if (_chit->delByNick(cIt->getNick()) == 1)
			temp = _chit->getFds(cIt->getFd());
		mList.insert(mList.end(), temp.begin(), temp.end());
		if (_chit->getUserSize() == 0)
			_chit = chList.erase(_chit);
	}
	sort(mList.begin(), mList.end());
	mList.erase(unique(mList.begin(), mList.end()), mList.end());
	std::string msg = "";
	if (_splitMsg.size() == 2)
		msg = _splitMsg[1];
	sendAll(mList, RPL_QUIT(cIt->getNick(), cIt->getNick(), cIt->getIp(), msg));
	cList.erase(cIt);
	return (1);
} 

int Command::ping(const Client &client) {
	if (_splitMsg.size() < 2)
		return (-1);
		
	sendFd(client.getFd(), RPL_PONG(_splitMsg[1]));
	return (1);
}

// privMsg #chName msg
int Command::privmsg(const Client &sender, const std::list<Channel> &chList) {
	if (_splitMsg.size() < 2)
		return (-1);
		
	std::string chName = _splitMsg[1];
	_coChit = checkValidChannel(_splitMsg[1], chList);
	if (_coChit == chList.end()) {
		sendFd(sender.getFd(), ERR_NOSUCHCHANNEL(sender.getNick(), chName));
		return (-1);
	}
	Channel channel = *_coChit;
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
int Command::privmsg(const Client &sender, const std::list<Client> &cList) {
	if (_splitMsg.size() < 2)
		return (-1);
		
	std::string cName = _splitMsg[1];
	_coCit = checkValidClient(cName, cList);
	if (_coCit == cList.end()) {
		sendFd(sender.getFd(), ERR_NOSUCHNICK(sender.getNick(), cName));
		return (-1);
	}
	sendFd(sender.getFd(), RPL_PRIVMSG(sender.getNick(), sender.getNick(), sender.getIp(), cName, _splitMsg[2]));
	return (1);
}

int Command::notice(const Client &sender, const std::list<Channel> &chList) {
	if (_splitMsg.size() < 2)
		return (-1);

	std::string chName = _splitMsg[1];
	_coChit = checkValidChannel(chName, chList);
	if (_coChit == chList.end()) {
		sendFd(sender.getFd(), ERR_NOSUCHCHANNEL(sender.getNick(), chName));
		return (-1);
	}
	Channel channel = *_coChit;
	if (channel.checkClient(sender.getNick()) == false) {
		sendFd(sender.getFd(), ERR_NOTONCHANNEL(sender.getNick(), chName));
		return (-1);
	}
	std::vector<int> fds = channel.getFds(sender.getFd());  
	sendAll(fds, RPL_NOTICE(sender.getNick(), sender.getNick(), sender.getIp(), chName, _splitMsg[2]));
	return (1);
}

int Command::notice(const Client &sender, const std::list<Client> &cList) {
	if (_splitMsg.size() < 2)
		return (-1);

	std::string cName = _splitMsg[1];
	_coCit = checkValidClient(cName, cList);
	if (_coCit == cList.end()) {
		sendFd(sender.getFd(), ERR_NOSUCHNICK(sender.getNick(), cName));
		return (-1);
	}
	sendFd(_coCit->getFd(), RPL_NOTICE(sender.getNick(), sender.getNick(), sender.getIp(), cName, _splitMsg[2]));
	return (1);
}

int Command::modeI(const Client &sender) {
	sendFd(sender.getFd(), RPL_MODE_I(sender.getNick(), sender.getIp()));
	return (1);
}

int Command::modeN(const Client &sender , const std::list<Channel> &chList) {
	if (_splitMsg.size() < 2)
		return (-1);

	std::string chName = _splitMsg[1];
	int cFd = sender.getFd();
	_coChit = checkValidChannel(chName, chList);
	if (_coChit == chList.end())
	{
		sendFd(cFd, ERR_NOSUCHNICK(sender.getNick(), chName));
		return (-1);
	}
	sendFd(cFd, RPL_MODE_N(sender.getNick(), chName));
	return (1);
}