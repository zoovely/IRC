#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "Define.hpp"

class Client{
    private:
        std::string _nick;
        std::string _user;
        std::string _ip;
        int _clientFd;
        bool _flag;

    public:
        Client(std::string nick, std::string user, std::string ip, int fd, bool flag);
        ~Client( void ){};

        void    setNick(std::string nick);
        const std::string getNick() const;
        const std::string getUser() const;
        const std::string getIp() const;
        bool getFlag() const;
        void setFlag(bool flag);
        void setInfo(std::string nick, std::string user, std::string ip, bool flag);
        int getFd() const;
        bool operator<(const Client& c) const;
};

#endif