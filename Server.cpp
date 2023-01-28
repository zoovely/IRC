#include <string>
#include "Server.hpp"

Server::Server(int portNum, std::string pwd) 
{
    _pwd = pwd;
    _serverFd = socket(PF_INET, SOCK_STREAM, 0);
    if (_serverFd == -1) 
        errorHandler("Error: socket open failed\n");
    else
        std::cout << "socket open success\n"; // 나중에 지우기
    _serverSin.sin_family = AF_INET;
    _serverSin.sin_addr.s_addr = INADDR_ANY;
    _serverSin.sin_port = htons(portNum);

    socklen_t _serverSinLen = sizeof(_serverSin);
    if (bind(_serverFd, (struct sockaddr*)&_serverSin, _serverSinLen) == -1) 
    {
        close(_serverFd);
        errorHandler("Error: bind failed\n");
    }
    else
        std::cout << "bind success\n"; // 나중에 지우기

    if (listen(_serverFd, 42) == -1) // how many backlog??
    {
        close(_serverFd);
        errorHandler("Error: listen failed\n");
    }
    else
        std::cout << "listen success\n"; // 나중에 지우기
	setPollFd(_serverFd, _serverFd, POLLIN, 0);
	for (int i = _serverFd + 1; i < OPEN_MAX; i++) // 0, 1, 2 에 대해서도 초기화 필요?
		setPollFd(i, -1, 0, 0);
}

int Server::acceptClient( void ) 
{
	struct sockaddr_in	sclient;
    socklen_t sclient_len = sizeof(sclient);
	int clientFd = accept(_serverFd, (struct sockaddr*) &sclient, &sclient_len);
	if (clientFd == -1) {
		std::cout << "Error: socket connect fail\n";
		return (-1); // 나중에 수정하기
	}
	std::cout << "client #" << clientFd << " connected\n"; // 나중에 지우기  
	setPollFd(clientFd, clientFd, POLLIN, 0);
	_poll[_serverFd].revents = 0;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	readClient(clientFd);
	// 패스워드 확인하고 맞으면 유저 네임, 닉네임 넣어서 만든 클라이언트 푸시
	// 닉네임이 중복인지 확인 후 중복이면 임의의 char붙여서 생성
	//checkNick();
	std::string nick;
	std::string user;
	Client client(nick, user, clientFd); // 닉, 유저 넣어서 만들기
	_clients.push_back(client); 
	return (1);
}

int Server::checkChannel(std::string channel)
{
    for (int i = 0; i != _channels.size(); i++)
    {
        if (_channels[i].getName() == channel)
            return (i);
    }
    return (-1);
}

bool Server::checkNick(std::string nick)
{
	return (true);
}



int Server::invite(Client client, std::string nickName, std::string channel) {
	// checkUser(nickName)
	//    return 1; // throw ERR_NOSUCHNICK;
    int i = checkChannel(channel);
    if (i == -1)
        return 1; // throw ERR_NOSUCHCHANNEL;
    if (_channels[i].invite(client, nickName))
        return 1; // 안에서 throw 하나요???
    return 0; // send invite msg to nickname to i
}

int	Server::join(Client client, std::string chName) 
{
	for (int i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getName() == chName)
			return (_channels[i].join(client));
	}
	Channel	ch(chName);
	_channels.push_back(ch);
	return (1);
}

int	Server::readClient(int fd) 
{
	// if문 main으로 옮김
	// readClient를 accept에서 호출
	int r = read(fd, _readBuf, BUF);
	if (r <= 0)
	{
		//client한테 read 실패 send 해주고
		//client 닫고 return -1
		std::cout << "Error: read fail\n";
		close(fd);
		setPollFd(fd, -1, 0, 0);
		return (-1);
	}
	std::cout << "buf : " << _readBuf << "\n"; // 나중에 지우기
	_poll[fd].revents = 0;
	std::memset(_readBuf, 0, BUF);
	return (1);
}

struct pollfd* Server::getPoll( void ) {
	return _poll;
}

int Server::getServerFd( void ) {
	return _serverFd;
}
// buf : CAP LS
// PASS 1234
// NICK lampolar
// USER lampolar lampolar 127.0.0.1 :김희선

std::vector<Channel> Server::getChannel( void )
{
	return _channels;
}

void Server::errorHandler(std::string msg) 
{
	std::cout << msg;
	std::exit(1);
}

void Server::setPollFd(int index, int fd, int events, int revents)
{
	_poll[index].fd = fd;
	_poll[index].events = events;
	_poll[index].revents = revents;
}