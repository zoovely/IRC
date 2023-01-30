#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client{
    private:
        std::string _nick;
        std::string _user;
        std::string _ip;
        int _clientFd;

    public:
        Client(std::string nick, std::string user, std::string ip, int fd);
        ~Client( void ){};

        void    setNick(std::string nick);
        const std::string getNick() const;
        const std::string getUser();
        const int getFd() const;
};

#endif