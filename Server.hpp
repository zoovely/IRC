#ifndef SERVER_H
#define SERVER_H

#include <list>
#include <poll.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include "Define.hpp"

#define BUF 4096
#define USER_MAX 100

class Server {
    private :
        int                  _serverFd;
        struct sockaddr_in   _serverSin;
        struct pollfd        _poll[USER_MAX];
        std::string          _pwd;
        std::list<Client>    _clients;
        std::list<Channel>   _channels;
        std::list<Client>::iterator     _cit;
        std::list<Channel>::iterator    _chit;
        char                 _readBuf[BUF];
        char                 _saveBuf[USER_MAX][BUF * 2];
        
    public :
        Server(int portNum, std::string pwd);

        std::list<Channel>::iterator checkChannel(std::string channel);
        int             acceptClient(void);
        int             readClient(int fd);
        void            errorHandler(std::string msg);
        void            executeCommand(int fd);

        struct pollfd*  getPoll(void);
        int             getServerFd(void);
        void            setPollFd(int index, int fd, int events, int revents);
        std::list<Client>::iterator getClientByFd(int fd);
};

#endif
