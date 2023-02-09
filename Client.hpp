#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "Define.hpp"

#define A_CONNECT 0
#define A_PASS 1
#define A_NICK 2
#define A_USER 3

class Client {
    private:
        std::string _nick;
        std::string _user;
        std::string _ip;
        int _clientFd;
        int _flag;

    public:
        Client(std::string nick, std::string user, std::string ip, int fd, int flag);

        void    setNick(std::string nick);
        const std::string getNick() const;
        const std::string getUser() const;
        const std::string getIp() const;
        int getFlag() const;
        void setFlag(int flag);
        void setInfo(std::string nick, std::string user, std::string ip, int flag);
        void setIp(std::string ip);
        void setUser(std::string user);
        int getFd() const;
        bool operator<(const Client& c) const;
};

#endif