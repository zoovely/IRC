#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "Server.hpp"

int main(void)
{
    int fd; 
    struct sockaddr_in	sin;
    struct sockaddr_in	sclient;

    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        std::cout << "error 발생 \n";
        return (1);}
    else
        std::cout << "잘열림\n";

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(6666);
    socklen_t sin_len = sizeof(sin);
    int i = bind(fd, (struct sockaddr*)&sin, sin_len);
    if (i == -1)
    {
        std::cout << "bind 실패\n";
        return (1);
    }
    else
        std::cout << "bind 성공\n";
    int j = listen(fd, 5);
    if (j == -1)
    {
        std::cout << "listen 실패\n";
        return (1);
    }
    else
        std::cout << "listen 성공\n";
    struct pollfd *polll;
    polll = new pollfd[fd + 5];
    int k = fd;
    polll[k].events = POLLIN;
    polll[k].revents = 0;
    polll[k++].fd = fd;

    while (k < fd + 4)
    {
        polll[k].fd = -1;
        polll[k].events = POLLIN;
        polll[k].revents = 0;
        k ++;
    }
    while (1){
        int ret;
        ret = poll(polll, fd + 5 , 1000);
        if (ret > 0)
        {
            socklen_t sclient_len = sizeof(sclient);
            if (polll[fd].revents == POLLIN)
            {
                int fd2 = accept(fd, (struct sockaddr*) &sclient, &sclient_len);
                if (fd2 == -1){
                    std::cout << "socket connect wrong\n";
                }
                else
                {
                    std::cout << fd2 << "socket connect success\n";                
                    polll[fd2].fd = fd2;
                    pdolll[fd2].events = POLLIN;
                }
                polll[fd].revents = 0;
                ret--;
            }
            int m = fd + 1;
            while (m < fd + 5 && ret > 0)
            {
                if (polll[m].revents == POLLIN)
                {
                    std::cout<< m << "shetre\n";
                    char buf[1000];
                    fcntl(polll[m].fd, F_SETFL, O_NONBLOCK);
                    int rresu = read(polll[m].fd, buf, 1000);
                    if (rresu <= 0)
                    {
                        std::cout << "read fail\n";
                        return (1);
                    }
                    std::cout << "buf : " << buf << std::endl;
                    polll[m].revents = 0;
                    ret--;
                }
                // if (polll[m].revents == POLLOUT)
                // {
                //     polll[m].revents = 0;
                //     ret--;
                // }
                m++;
            }
        }
        else
            std::cout << "poll nothing return\n";
    }
}