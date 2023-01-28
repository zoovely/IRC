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
        ~Server( void ){};
        int checkChannel(std::string channel);
        int acceptClient( void ); // 뭐 넘겨줘야 하지..?, 닉네임 체크 + 패스워드 체크
        int readClient(int fd);
        bool checkNick(std::string nick); // 클라이언트 추가할 때 닉네임 중복 확인용 <- 중복이면 닉네임에 뭐 달아주고 경고문 뱉어줘야 함
        void errorHandler(std::string msg);
        int invite(Client client, std::string nickName, std::string channel);int join(Client client, Channel channel);
        int join(Client client, std::string chName);

        std::vector<Channel> getChannel();
        struct pollfd* getPoll( void );
        int getServerFd( void );
        void setPollFd(int index, int fd, int events, int revents);
};
#endif

// <cstdlib> int atoi( const char* str ); long is also possible
                                        // long long = c++11;

