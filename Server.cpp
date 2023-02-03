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
	// readClient(clientFd);
	return (1);
}

int Server::checkChannel(std::string channel)
{
    for (size_t i = 0; i != _channels.size(); i++)
    {
        if (_channels[i].getName() == channel)
            return (i);
    }
    return (-1);
}

bool Server::checkNick(std::string nick)
{
	(void)nick;
	return (true);
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
	std::cout << "buf : " << fd << "+" << _readBuf << "\n"; // 나중에 지우기
	executeCommand(fd);
	_poll[fd].revents = 0;
	std::memset(_readBuf, 0, BUF);
	return (1);
}

int Server::getClientByFd(int fd) {
	for(size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i].getFd() == fd)
			return (i);
	}
	return (-1);
}

void	Server::executeCommand(int fd) {
	Command	com(_readBuf);
	int	type = com.checkMsgType();
	int	cIdx = getClientByFd(fd);
	switch (type)
	{
		case CONNECT:
			com.connect(fd, _pwd, _clients);
			break;
		case JOIN:
			com.join(_clients[cIdx], _channels);
			break;
		case PART:
			com.part(_clients[cIdx], _channels);
			break;
		case INVITE:
			com.invite(_clients[cIdx], _channels, _clients);
			break;
		case KICK:
			com.kick(_clients[cIdx], _channels);
			break;
		case NICK:
			com.nick(_clients[cIdx], _clients);
			break;
		case LIST:
			com.list(_clients[cIdx], _channels);
			break;
		case WHOIS:
			com.whois(_clients[cIdx], _clients);
			break;
		case QUIT:
			com.quit(_clients[cIdx], _channels, _clients);
			break;
		case PING:
			com.ping(_clients[cIdx]);
			break;
		case OP:
			com.op(_clients[cIdx], _channels);
			break;
		case DEOP:
			com.deop(_clients[cIdx], _channels);
			break;
		case PRIVMSG:
			com.privmsg(_clients[cIdx], _clients);
			break;
		case PRIVCH:
			com.privmsg(_clients[cIdx], _channels);
			break;
		case NOTICE:
			com.notice(_clients[cIdx], _clients);
			break;
		case NOTICH:
			com.notice(_clients[cIdx], _channels);
			break;
		case MODE_I:
			com.modeI(_clients[cIdx]);
			break;
		default:
			break;
	}
}

struct pollfd* Server::getPoll( void ) {
	return _poll;
}

int Server::getServerFd( void ) {
	return _serverFd;
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
