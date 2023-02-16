#include <poll.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Server.hpp"

int checkArg(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Error : Wrong Argument\n";
        std::cout << "Usage: ./irserve <portnum> <password>\n";
        return (-1);
    }
    for (int i = 0; argv[1][i] != '\0'; i++)
    {
        if (argv[1][i] < '0' || argv[1][i] > '9')
        {
            std::cout << "Error : Wrong port num\n";
            return (-1);
        }
    }
    int portNum = std::atoi(argv[1]);
    if (portNum < 0 || portNum > 65535) {
        std::cout << "Error : Wrong port num\n";
        return (-1);
    }
    return (portNum);
}

int main(int argc, char *argv[]) {
    int portNum = checkArg(argc, argv);
    if (portNum == -1)
        return (1);
    
    Server server(portNum, argv[2]);
    int ret;
    int serverFd = server.getServerFd();
    struct pollfd *serverPoll = server.getPoll();

    std::cout << " _     _   ___ __ __ _  ___   __ \n";
    std::cout << "| |   / \\ |_ / \\ V /| || o \\ / _|\n";
    std::cout << "| |_ | o | /(_  \\ / | ||   /( (_ \n";
    std::cout << "|___||_n_|/___| |_| |_||_|\\\\ \\__|\n\n";
    
    while (1)
    {
        ret = poll(serverPoll, 100, 500);
        if (ret > 0)
        {
            if (serverPoll[serverFd].revents & POLLIN)
            {
                if (server.acceptClient() == 1)
                    ret--;
            }
            for (int i = serverFd + 1; i < USER_MAX && ret > 0; i++)
            {
                if (serverPoll[i].revents & POLLIN || serverPoll[i].revents & POLLHUP)
	            {
                    if (server.readClient(i) == 1)
                        ret--;
                }
            }
        }
    }
    return (0);
}