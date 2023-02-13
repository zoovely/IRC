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
    if (argc != 3) {
        std::cout << "Error : Wrong Argument\n";
        std::cout << "Usage: ./irserve <portnum> <password>\n";
        return (1);
    }
    int portNum = std::atoi(argv[1]);
    if (portNum < 0 || portNum > 65535) {
        std::cout << "Error : Wrong port num\n";
        return (1);
    }

    Server server(portNum, argv[2]);
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
            for (int i = serverFd + 1; i < USER_MAX && ret > 0; i++)
            {
                if (serverPoll[i].revents == POLLIN)
	            {
                    if (server.readClient(i) == 1)
                        ret--;
                }
            }
        }
    }
    return (0);
}