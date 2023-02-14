#include <string>
#include "Server.hpp"

Server::Server(int portNum, std::string pwd)
{
    _pwd = pwd;
    _serverFd = socket(PF_INET, SOCK_STREAM, 0);
    if (_serverFd == -1) 
        errorHandler("Error: socket open failed\n");
    _serverSin.sin_family = AF_INET;
    _serverSin.sin_addr.s_addr = INADDR_ANY;
    _serverSin.sin_port = htons(portNum);

    socklen_t _serverSinLen = sizeof(_serverSin);
    if (bind(_serverFd, (struct sockaddr*)&_serverSin, _serverSinLen) == -1) 
    {
        close(_serverFd);
        errorHandler("Error: bind failed\n");
    }
    if (listen(_serverFd, 42) == -1)
    {
        close(_serverFd);
        errorHandler("Error: listen failed\n");
    }
	setPollFd(_serverFd, _serverFd, POLLIN, 0);
	for (int i = _serverFd + 1; i < USER_MAX; i++)
		setPollFd(i, -1, 0, 0);
	std::memset(_saveBuf, 0, BUF * 2 * USER_MAX);
}

std::list<Channel>::iterator Server::checkChannel(std::string chName)
{
	for (_chit = _channels.begin(); _chit != _channels.end(); _chit++)
	{
		if (_chit->getName() == chName)
			break ;
	}
	return (_chit);
}

int Server::acceptClient( void ) 
{
	struct sockaddr_in	sclient;
    socklen_t sclient_len = sizeof(sclient);
	int clientFd = accept(_serverFd, (struct sockaddr*) &sclient, &sclient_len);
	if (clientFd == -1) {
		std::cout << "Error: socket connect fail\n";
		return (-1);
	}
	setPollFd(clientFd, clientFd, POLLIN | POLLHUP, 0);
	_poll[_serverFd].revents = 0;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);
	return (1);
}

int	Server::readClient(int fd) 
{
	std::memset(_readBuf, 0, BUF);
	int r = recv(fd, _readBuf, BUF, MSG_DONTWAIT);
	if (r < 0)
	{
		std::cout << "Error: read fail\n";
		close(fd);
		setPollFd(fd, -1, 0, 0);
		return (-1);
	} else if (r == 0) 
	{
		std::memset(_saveBuf[fd], 0, BUF * 2);
		std::strcat(_saveBuf[fd], "QUIT :disconnected\r\n");
		std::cout << "========== recv client " << fd << " ==========\n";
		std::cout << _saveBuf[fd] << "\n\n";
		executeCommand(fd);
		setPollFd(fd, -1, 0, 0);
		std::memset(_saveBuf[fd], 0, BUF * 2);
		return (1);
	}
	if (_readBuf[r - 2] == '\r' && _readBuf[r - 1] == '\n')
	{
		std::strcat(_saveBuf[fd], _readBuf);
		std::cout << "========== recv client " << fd << " ==========\n";
		std::cout << _saveBuf[fd] << "\n\n";
		executeCommand(fd);
		_poll[fd].revents = 0;
		std::memset(_saveBuf[fd], 0, BUF * 2);
	}
	else
		std::strcat(_saveBuf[fd], _readBuf);
	return (1);
}

void Server::errorHandler(std::string msg) 
{
	std::cout << msg;
	std::exit(1);
}

void	Server::executeCommand(int fd) {
	Command	com(_saveBuf[fd]);
	int	type = com.checkMsgType();
	
	_cit = getClientByFd(fd);
	switch (type)
	{
		case CONNECT:
			com.connect(fd, _pwd, _clients);
			break;
		case PASS:
			com.pass((*_cit), _pwd, _clients);
			break;
		case JOIN:
			com.join((*_cit), _channels);
			break;
		case PART:
			com.part((*_cit), _channels);
			break;
		case INVITE:
			com.invite((*_cit), _channels, _clients);
			break;
		case KICK:
			com.kick((*_cit), _channels);
			break;
		case NICK:
			com.nick((*_cit), _clients, _channels);
			break;
		case USER:
			com.user((*_cit));
			break;
		case LIST:
			com.list((*_cit), _channels);
			break;
		case WHOIS:
			com.whois((*_cit), _clients);
			break;
		case QUIT:
			com.quit(_cit, _channels, _clients);
			break;
		case PING:
			com.ping((*_cit));
			break;
		case OP:
			com.op((*_cit), _channels);
			break;
		case DEOP:
			com.deop((*_cit), _channels);
			break;
		case PRIVMSG:
			com.privmsg((*_cit), _clients);
			break;
		case PRIVCH:
			com.privmsg((*_cit), _channels);
			break;
		case NOTICE:
			com.notice((*_cit), _clients);
			break;
		case NOTICH:
			com.notice((*_cit), _channels);
			break;
		case MODE_I:
			com.modeI((*_cit));
			break;
		case MODE_N:
			com.modeN((*_cit), _channels);
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

void Server::setPollFd(int index, int fd, int events, int revents)
{
	_poll[index].fd = fd;
	_poll[index].events = events;
	_poll[index].revents = revents;
}

std::list<Client>::iterator Server::getClientByFd(int fd) {
	for (_cit = _clients.begin(); _cit != _clients.end(); _cit++)
	{
		if (_cit->getFd() == fd)
			break;
	}
	return (_cit);
}