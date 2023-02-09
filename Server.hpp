#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <poll.h>
#include <cstring>

#include "Client.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include "Define.hpp"

#define BUF 4096

class Server {
    private :
        int                  _serverFd;
        struct sockaddr_in   _serverSin;
        struct pollfd        _poll[OPEN_MAX];
        std::string          _pwd;
        std::vector<Client>  _clients;
        std::vector<Channel> _channels;
        char                 _readBuf[BUF];
        
    public :
        Server(int portNum, std::string pwd);
        int checkChannel(std::string channel);
        int acceptClient( void );
        int readClient(int fd);
        void errorHandler(std::string msg);
        void executeCommand(int fd);

        struct pollfd* getPoll( void );
        int getServerFd( void );
        void setPollFd(int index, int fd, int events, int revents);
        int getClientByFd(int fd);
};

#endif
