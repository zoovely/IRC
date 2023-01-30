#include "Command.hpp"

Command::Command(std::string msg) {
	_msg = msg;
	splitMsg();
}

void Command::splitMsg(void) {
	std::string::size_type pos = _msg.find(" " , 0);
	std::string::size_type newpos = _msg.find("\n", 0);
	int i = 0;
	while (pos != std::string::npos && newpos != std::string::npos)
	{
		if (pos > newpos)
			pos = newpos;
		_splitMsg.push_back(_msg.substr(i, pos - i));
		i = pos + 1;
		pos = _msg.find(" ", i);
		newpos = _msg.find("\n", i);
	}
	_splitMsg.push_back(_msg.substr(i, _msg.length() - i));
	return ;
}

int	Command::checkMsgType(void) {
	std::string	typeList[] = {"CAP", "JOIN", "PART", "INVITE", "KICK", "NICK", "LIST", "WHOIS", "QUIT", "PING", "MODE", "PRIVMSG", "NOTICE"};
	for (int i = 0; i < typeList->length(); i++) {
		if (_splitMsg[0].find(typeList[i]) != std::string::npos) {
			switch (i)
			{
				case 10:
					if (_splitMsg[2].find("+o", 0) != std::string::npos)
						return (OP);
					else if (_splitMsg[2].find("-o", 0) != std::string::npos)
						return (DEOP);
					break;
				case 11:
					if (_splitMsg[1].find("#", 0) == std::string::npos)
						return (PRIVMSG);
					else
						return (PRIVCH);
				case 12:
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

void Command::send(int fd, std::string str) {
	
}

//channel 이름으로 list에서 찾는 함수
int Command::checkValidChannel(const std::string chName, const std::vector<Channel> &chList) {
	for (int i = 0; i < chList.size(); i++) {
		if (chList[i].getName() == chName) {
			return (i);
		}
	}
	return (-1);
}

//nick네임으로 client list에서 찾는 함수
int Command::checkValidClient(const std::string nick, const std::vector<Client> &cList) {
	for (int i = 0; i < cList.size(); i++) {
		if (cList[i].getNick() == nick) {
			return (i);
		}
	}
	return (-1);
}

int	Command::checkValidNick(const std::string nick) {
	// 숫자, 영어 대소문자, _ 만 가능
	// 커맨드 16개 검사
	// 안되는 고면 -1 리턴 되면 1 리턴
}

//command list 
int Command::connect(int fd, std::string pwd, std::vector<Client> &cList) {
	int i;
	std::string nick = "";
	std::string	ip = "";
	
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("PASS") == 0)
			break;
	}
	if (i == _splitMsg.size() || _splitMsg[i + 1].compare(pwd) != 0) {
		// 에러 메시지 전송
		return (-1);
	}
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("NICK") == 0)
			break;
	}
	if (i == _splitMsg.size() || checkValidNick(_splitMsg[i + 1]) == -1 || \
		checkValidClient(_splitMsg[i + 1], cList) != -1) {
		// 에러 메시지 전송
		return (-1);
	}
	nick = _splitMsg[i + 1];
	for (i = 0; i < _splitMsg.size(); i++) {
		if (_splitMsg[i].compare("USER") == 0)
			break;
	}
	if (i != _splitMsg.size()) {
		ip = _splitMsg[i + 3];
	}
	Client	nClient(nick, _splitMsg[i + 4].erase(0, 1), ip, fd);
	cList.push_back(nClient);
	// welcome msg 전송
	
	// CAP LS
	// PASS 1234
	// NICK lampolar
	// USER lampolar lampolar 127.0.0.1 :김희선

} // 비번인증, 닉네임 중복검사 내부에서 필요& & welcome msg전송

int Command::join(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	for(int i = 0; i < chList.size(); i++) {
		if (chList[i].getName() == chName) {
			chList[i].addUser(client);
			return (1); // 굳이 int 돌려줘야 하는지?
		}
	}
	Channel ch(client, chName);
	chList.push_back(ch);
	// send 채팅방에 있는 애들한테 다 알려줘야 함
	return (1);
}

void Command::delChannel(std::vector<Channel> &chList, Channel& channel) {
	if (chList.size() == 1)
	{
		chList.clear();
		return ;
	}
	for (std::vector<Channel>::iterator it = chList.begin(); it != chList.end(); it++)
	{
		if ((*it).getName() == channel.getName())
			chList.erase(it);
	}
	return ;
}

int Command::part(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx != -1) {
		if (chList[chIdx].checkClient(client.getNick())) {
			chList[chIdx].delUser(client);
			if (chList[chIdx].getUserSize() == 0) // channel에 남은 사람이 있는지 확인
				delChannel(chList, chList[chIdx]);//chIdx
			return (1); // send?
		}
	}
	return (-1); // send??
}; 

// invite nick chName
int Command::invite(const Client &client, const std::vector<Channel> &chList, const std::vector<Client> &cList) {
	int chIdx = checkValidChannel(_splitMsg[2], chList);
	if (chIdx == -1) // 그 채널이 없을 때 에러처리?
		return (-1);
	if (chList[chIdx].checkAuth(client) == false) // op 권한이 없음
		return (-1); //ERR_CHANOPRIVSNEEDED
	return (checkValidClient(_splitMsg[1], cList)); // 초대가 가능한 유효한 유저이면 그 인덱스 아니면 -1 반환
}

int Command::kick(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	std::string nick = _splitMsg[2];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1)
		return (-1); // 해당 channel이 없을 경우
	Channel channel = chList[chIdx];
	if (channel.checkAuth(client) == false)
		return (-1); // 요청한 유저의 권한이 없을 경우
	if (channel.checkClient(nick) == false)
		return (-1); // 퇴장할 유저가 channel에 속해있지 않을 경우
    // nick에 대해서 강제 퇴장 메세지를 channel에 전송
	channel.delByNick(nick); // 유저 퇴장시키기
	if (channel.getUserSize() == 0)
		delChannel(chList, channel); //유저 퇴장후 남은 유저가 없을 경우 채널 삭제
    return (1);
}

int Command::op(const Client &client, std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	std::string nick = _splitMsg[3];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1)
		return (-1); // channel이 없을 경우
	Channel channel = chList[chIdx];
	if (channel.checkAuth(client) == false)
		return (-1); // 요청한 유저가 oper권한이 없을 경우
	if (channel.checkClient(nick) == false)
		return (-1); // oper가 될 유저가 channel에 없을 경우
	channel.opUser(nick); // 유저에게 op 부여
	//정상 작동 send
	return (1);
}

int Command::deop(const Client &client, std::vector<Channel> &chList){
	std::string chName = _splitMsg[1];
	std::string nick = _splitMsg[3];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1)
		return (-1); //channel이 없을 경우
	Channel channel = chList[chIdx];
	if (channel.checkAuth(client) == false)
		return (-1); // 요청한 유저가 oper권한이 없을 경우 
	if (channel.checkClient(nick) == false)
		return (-1); // deop할 유저가  channel에 속해있지 않을 경우
	channel.deopUser(nick); //nick에게서 op권한 뺏기
	//정상 작동 send
	return (1);
}

// nick change nick
int Command::nick(Client &client, const std::vector<Client> &cList) {
	std::string nickName = _splitMsg[1];
	if (checkValidClient(nickName, cList) != -1)
		return (-1); // 중복된 닉네임이 있을 경우
	if (checkValidNick(nickName) == -1)
		return (-1); // 유효하지 않은 nickname이 들어올 경우
	client.setNick(nickName);
	return (1); // 중복되지 않음
}

// list
int Command::list(const Client &client, const std::vector<Channel> &chList) {
	std::vector<Channel>::const_iterator coit = chList.begin();
	for (coit; coit != chList.end(); coit++)
	{
		continue;
		//client에게 chlist info에 대해서 send
	}
	return (1); //정상 작동
}

// whois nick
int Command::whois(const Client &client, const std::vector<Client> &cList) {
	if (_splitMsg.size() == 1) {
		// 내 정보 센드
		return (1);
	}
	int cIdx = checkValidClient(_splitMsg[1], cList);
	if (cIdx != -1) {
		Client client = cList[cIdx];
		// 이 클라이언트 정보 센드
	}
	return (-1); // 이런 nick 가진 유저 못찾으면~!
}

// privMsg #chName msg
int Command::privmsg(const Client &sender, const std::vector<Channel> &chList) {
	int chIdx = checkValidChannel(_splitMsg[1], chList);
	if (chIdx == -1)
		return (-1); // 채널 없는거~!
	Channel channel = chList[chIdx];
	if (!channel.checkClient(sender.getNick()))
		return (-1); // 센더가 그 채널에 없는거~!
	std::vector<int> fds = channel.getFds(sender.getFd());  
	//send에 fds에 대해서 msg를 보내주기
};

// privmsg nick msg //
int Command::privmsg(const Client &sender, const std::vector<Client> &cList) {
	int cIdx = checkValidClient(_splitMsg[1], cList);
	if (cIdx == -1)
		return (-1); // 그런 유저가 없답니다!
	// send(cIdx, "msg", 10); 머 이런식으로 cIdx==fd니까 이렇게 보내면 될듯은 함.
	// 이 receiver는 
	return (1);
}

int Command::notice(const Client &sender, const std::vector<Channel> &chList) {
	std::string chName = _splitMsg[1];
	int chIdx = checkValidChannel(chName, chList);
	if (chIdx == -1)
		return (-1); // channel이 없을 경우
	Channel channel = chList[chIdx];
	if (channel.checkClient(sender.getNick()) == false)
		return (-1); // channel에 nick이 속해있지 않을 경우
	// send 해주기
	// send시 :nick!nick@ip 달고 시작
	return (1);
}

int Command::notice(const Client &sender, const std::vector<Client> &cList) {
	std::string cName = _splitMsg[1];
	int cIdx = checkValidClient(cName, cList);
	if (cIdx == -1)
		return (-1); // notice를 받을 해당 유저가 없을 경우 
	//send해주기 
	return (1);
}

int Command::quit(Client &client, std::vector<Channel> &chList, std::vector<Client> &cList) {
	for (int i = 0; i < chList.size(); i++) {
		chList[i].delByNick(client.getNick());
		// 이 방에 포함된 모든 사람에게 send 메시지 (quit 메시지 포함)
	}
	cList.erase(checkValidClient(client.getNick(), cList) + cList.begin());
	// send quit msg;
} // channel에 남은 사람이 있는지 확인 

int Command::ping(const Client &client) {
	// send PONG :_splitmsg[1] 
}
