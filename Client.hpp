#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client{
    private:
        std::string _nick;
        std::string _user;
        int _clientFd;

    public:
        Client(std::string nick, std::string user, int fd);
        ~Client( void ){};

        void    setNick(std::string nick);
        void    setUser(std::string user);
        std::string getNick();
        std::string getUser();
};

#endif