#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "Server.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3) //err msg
        return (1);
    // argv 정상값인지 확인 필요 
    // port num: range 어떻게 되지?  port번호는 0 ~ 65535번까지 사용할 수 있습니다. 일반적으로 unsigned short의 2바이트 정수형을 사용합니다.
    //                            최근에 IANA에서 관리하는 할당된 포트 범위는 범위 0-1023으로 확장되었습니다.
    // 비밀번호는 어디까지 허용?
    
    Server server(std::atoi(argv[1]), argv[2]);
    int ret;
    int serverFd = server.getServerFd();
    struct pollfd *serverPoll = server.getPoll();
    
    while (1)
    {
        ret = poll(serverPoll, 100, 500);
        if (ret > 0)
        {
            if (serverPoll[serverFd].revents == POLLIN)
            {
                if (server.acceptClient() == 1)
                    ret--;
            }
            for (int i = serverFd + 1; i < OPEN_MAX && ret > 0; i++)
            {
                if (serverPoll[i].revents == POLLIN)
	            {
                    if (server.readClient(i) == 1)
                        ret--;
                }
            }
        }
        // else
        //     std::cout << "poll nothing return\n"; //나중에 지우기?
    }
}