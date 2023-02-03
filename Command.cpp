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
		_splitMsg.push_back(_msg.substr(colone, _msg.length() - colone - 2));
	else
		_splitMsg.push_back(_msg.substr(i, _msg.length() - i - 2));
	return ;
}

int	Command::checkMsgType(void) {
	std::string typeList[] = {"CAP", "PASS", "JOIN", "PART", "INVITE", "KICK", "NICK", "LIST", "WHOIS", "QUIT", "PING", "MODE", "PRIVMSG", "NOTICE"};
	for (size_t i = 0; i < sizeof(typeList)/sizeof(std::string); i++) {
		if (_splitMsg[0].find(typeList[i]) != std::string::npos) {
			switch (i)
			{
				case 11:
					if (_splitMsg[2].find("+o", 0) != std::string::npos)
						return (OP);
					else if (_splitMsg[2].find("-o", 0) != std::string::npos)
						return (DEOP);
					else if (_splitMsg[2].find("+i", 0) != std::string::npos)
						return (MODE_I);
					else if (_splitMsg[1].find("#", 0) != std::string::npos)
						return (MODE_N);
					break;
				case 12:
					if (_splitMsg[1].find("#", 0) == std::string::npos)
						return (PRIVMSG);
					else
						return (PRIVCH);
				case 13:
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
	std::cout << "send fd :" << fd << " msg :" << str << "\n";
	int ret = send(fd, str.c_str(), str.size(), MSG_DONTWAIT);
	if (ret == -1) {
		std::cerr << str.c_str() << "\n";
		std::cerr << "send failed\n";
	}
}

void Command::sendAll(std::vector<int> fds, std::string str) {
	for (size_t i = 0; i < fds.size(); i++)
		sendFd(fds[i], str);
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
	std::string	cmdList[] = {"CAP", "JOIN", "PART", "INVITE", "KICK", "NICK", "LIST", "WHOIS", "QUIT", "PING", "MODE", "PRIVMSG", "NOTICE"};
	for (size_t i = 0; i < cmdList->size(); i++) { //cmdlist와 같은지 확인
		if (nick == cmdList[i])
			return (-1);
	}
	for (size_t i = 0; i < nick.size(); i++) {
		if ((!std::isalnum(nick[i]) && nick[i] != '-' && nick[i] != '_'))
			return (-1); //숫자 & 영어 & - & _ 다 아니면!
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
	Client nClient("", "", "", fd, true);
	cList.push_back(nClient);
	if (_splitMsg.size() != 2 && _splitMsg[2] == "PASS") { // 
		pass(cList.back(), pwd, cList, cList.size() - 1);
	}
	return (1);
}

int Command::pass(Client &client, std::string pwd, std::vector<Client> &cList, int idx) {
	size_t i;
	std::string nick = "";
	std::string	ip = "";
	if (client.getFlag() == false)
		return (1);
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("PASS") == 0)
			break;
	}
	if (i == _splitMsg.size() || _splitMsg[i + 1].compare(pwd) != 0) {
		sendFd(client.getFd(), ERR_PASSWDMISMATCH);
		cList.erase(idx + cList.begin());
		return (-1);
	}
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("NICK") == 0)
			break;
	}
	if (i == _splitMsg.size()) //nick 없음
	{
		sendFd(client.getFd(), ERR_NONICKNAMEGIVEN);
		cList.erase(idx + cList.begin());
		return (-1);
	}
	nick = _splitMsg[i + 1];
	if (checkValidNick(nick) == -1) //nick 규칙 안맞음
	{
		sendFd(client.getFd(), ERR_ERRONEUSNICKNAME(nick));
		cList.erase(idx + cList.begin());
		return (-1);
	}
	if (checkValidClient(nick, cList) != -1) //이미 존재하는 이름
		sendFd(client.getFd(), ERR_NICKNAMEINUSE(nick));
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("USER") == 0)
			break;
	}
	if (i == _splitMsg.size()) {
		// err msg type? user keyword가 안들어 왔을 경우?
		cList.erase(idx + cList.begin());
		return (-1);
	}
	ip = _splitMsg[i + 3];
	// welcome msg 001, 002 전송
	
	client.setInfo(nick, _splitMsg[i + 4].erase(0, 1), ip, false);
	sendFd(client.getFd(), RPL_WELCOME(nick));
	sendFd(client.getFd(), RPL_YOURHOST(nick));
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
	// std::cout << RPL_NAMREPLY(client.getNick(), chName, chList[i].getUsersNames()) << "\n"; // for test
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
	if (!ch.checkClient(client.getNick())) { // 클라이언트가 그 채널에 없는 경우
		sendFd(client.getFd(), ERR_NOTONCHANNEL(client.getNick(), chName));
		return (-1);
	}
	ch.delUser(client);
	sendFd(client.getFd(), RPL_PART(client.getNick(), client.getIp(), chName));
	if (ch.getUserSize() == 0) { // channel에 남은 사람이 있는지 확인
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
	if (!chList[chIdx].checkAuth(client)) { // op 권한이 없음
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
	// 유저가 이미 있는 채널 인바이트 에러 처리 해야한댔나?
	return (1);
}

// kick chName nick
int Command::kick(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	std::string target = _splitMsg[2];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1) { // 해당 채널이 없는 경우
		sendFd(client.getFd(), ERR_NOSUCHCHANNEL(client.getNick(), chName));
		return (-1);
	}
	Channel &channel = chList[chIdx];
	if (!chList[chIdx].checkAuth(client)) { // op 권한이 없음
		sendFd(client.getFd(), ERR_CHANOPRIVSNEEDED(client.getNick(), chName));
		return (-1);
	}
	if (!channel.checkClient(target)) { // 퇴장할 유저가 그 채널에 없는 경우
		sendFd(client.getFd(), ERR_NOTONCHANNEL(client.getNick(), chName));
		return (-1);
	}
    // nick에 대해서 강제 퇴장 메세지를 channel에 전송
	std::string msg = "";
	if (_splitMsg.size() > 3)
		msg = _splitMsg[3]; // msg가 이게 맞는지?
	sendFd(client.getFd(), RPL_KICK(client.getNick(), client.getNick(), client.getIp(), chName, target, msg));
	sendAll(channel.getFds(client.getFd()), RPL_KICK(client.getNick(), client.getNick(), client.getIp(), chName, target, msg));
	channel.delByNick(target); // 유저 퇴장시키기
	if (channel.getUserSize() == 0)
		delChannel(chList, channel); //유저 퇴장후 남은 유저가 없을 경우 채널 삭제
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
		return (-1); // channel이 없을 경우
	}
	Channel &channel = chList[chIdx];
	if (channel.checkAuth(client) == false)
	{
		msg = ERR_CHANOPRIVSNEEDED(client.getNick(), chName);
		sendFd(cFd, msg);	
		return (-1); // 요청한 유저가 oper권한이 없을 경우
	}
	if (channel.checkClient(nick) == false)
	{
		msg = ERR_USERNOTINCHANNEL(nick, chName);
		sendFd(cFd, msg);	
		return (-1); // oper가 될 유저가 channel에 없을 경우
	}
	else 
	{ //정상작동
		channel.opUser(nick); // 유저에게 op 부여
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
		return (-1); //channel이 없을 경우
	}
	Channel &channel = chList[chIdx];
	if (channel.checkAuth(client) == false)
	{
		msg = ERR_CHANOPRIVSNEEDED(client.getNick(), chName);
		sendFd(cFd, msg);
		return (-1); // 요청한 유저가 oper권한이 없을 경우 
	}
	if (channel.checkClient(nick) == false)
	{
		msg = ERR_USERNOTINCHANNEL(nick, chName);
		sendFd(cFd, msg);	
		return (-1); // deop할 유저가  channel에 속해있지 않을 경우
	}
	else
	{
		channel.deopUser(nick); //nick에게서 op권한 뺏기
		msg = RPL_DEOP(client.getNick(), client.getNick(), client.getIp(), chName, nick);
		sendAll(channel.getFds(cFd), msg);
		//정상 작동 send
	}
	sendFd(cFd, msg);
	return (1);
}

// nick change nick
int Command::nick(Client &client, std::vector<Client> &cList) {
	std::string nickName = _splitMsg[1];
	int cFd = client.getFd();
	std::string msg;

	if (client.getFlag()) {
		sendFd(cFd, ERR_PASSWDMISMATCH);
		cList.erase(getClientByFd(cFd, cList) + cList.begin());
		return (-1);
	} 
	if (nickName.empty() == true)
	{
		sendFd(cFd, ERR_NONICKNAMEGIVEN);
		return (-1); // 닉네임이 비어있을 경우
	}
	if (checkValidClient(nickName, cList) != -1)
	{
		msg = ERR_NICKNAMEINUSE(nickName);
		sendFd(cFd, msg);
		return (-1); // 중복된 닉네임이 있을 경우
	}
	if (checkValidNick(nickName) == -1)
	{	
		msg = ERR_ERRONEUSNICKNAME(nickName);
		sendFd(cFd, msg);
		return (-1); // 유효하지 않은 nickname이 들어올 경우
	}
	else 
	{
		msg = RPL_NICK(client.getNick(), client.getNick(), client.getIp(), nickName);
		sendFd(cFd, msg);
		client.setNick(nickName);
	}
	return (1); // 중복되지 않음
	// ERR_NICKCOLLISION (436) : 뭔지 모르겠음
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
		//client에게 chlist info에 대해서 send
	}
	msg = RPL_LISTEND(client.getNick());
	sendFd(cFd, msg);
	return (1); //정상 작동
}

// whois nick
int Command::whois(const Client &client, const std::vector<Client> &cList) {
	int cFd = client.getFd();
	std::string target = _splitMsg[1];
	int cIdx = checkValidClient(target, cList);
	if (cIdx == -1) {
		sendFd(cFd, ERR_NOSUCHNICK(client.getNick(), target));
		return (-1); // 이런 nick 가진 유저 못찾으면~!
	}
	// 이 클라이언트 정보 센드
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
		mList.insert(mList.end(), temp.begin(), temp.end()); // 본인 빼고 채널 사람들 넣기
		if (chList[i].getUserSize() == 0)
			chList.erase(chList.begin() + i); // channel에 남은 사람이 있는지 확인
	}
	sort(mList.begin(), mList.end());
	mList.erase(unique(mList.begin(), mList.end()), mList.end()); // 중복 제거
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
		return (-1); // 채널 없는거~!
	}
	Channel channel = chList[chIdx];
	if (!channel.checkClient(sender.getNick())) {
		sendFd(sender.getFd(), ERR_NOTONCHANNEL(sender.getNick(), chName));
		return (-1); // 센더가 그 채널에 없는거~!
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
		return (-1); // 그런 유저가 없답니다!
	}
	// sendFd(sender.getFd(), RPL_PRIVMSG(sender.getNick(), sender.getNick(), sender.getIp(), cName, _splitMsg[2])); 본인용
	sendFd(cList[cIdx].getFd(), RPL_PRIVMSG(sender.getNick(), sender.getNick(), sender.getIp(), cName, _splitMsg[2]));
	return (1);
}

int Command::notice(const Client &sender, const std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1) {
		sendFd(sender.getFd(), ERR_NOSUCHCHANNEL(sender.getNick(), chName));
		return (-1); // channel이 없을 경우	
	}
	Channel channel = chList[chIdx];
	if (channel.checkClient(sender.getNick()) == false) {
		sendFd(sender.getFd(), ERR_NOTONCHANNEL(sender.getNick(), chName));
		return (-1); // channel에 nick이 속해있지 않을 경우
	}
	// sendFd(sender.getFd(), RPL_NOTICE(sender.getNick(), sender.getNick(), sender.getIp(), chName, _splitMsg[2])); 본인용
	// 보낸사람 fd에만 써줘도 톡방 전체에 전해지나??? 아니다에 한표
	std::vector<int> fds = channel.getFds(sender.getFd());  
	sendAll(fds, RPL_NOTICE(sender.getNick(), sender.getNick(), sender.getIp(), chName, _splitMsg[2]));
	return (1);
}

int Command::notice(const Client &sender, const std::vector<Client> &cList) {
	std::string cName = _splitMsg[1];
	int cIdx = checkValidClient(cName, cList);
	if (cIdx == -1) {
		sendFd(sender.getFd(), ERR_NOSUCHNICK(sender.getNick(), cName));
		return (-1); // notice를 받을 해당 유저가 없을 경우
	}
	// send해주기 (본인한테도?)
	// sendFd(sender.getFd(), RPL_NOTICE(sender.getNick(), sender.getNick(), sender.getIp(), cName, _splitMsg[2])); 본인한테 보내는거
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
		return (-1); // channel이 없을 경우
	}
	sendFd(cFd, RPL_MODE_N(sender.getNick(), chName));
	return (1);
}