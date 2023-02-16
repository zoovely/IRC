#ifndef CHANNEL_H
#define CHANNEL_H

#include <map>
#include <vector>
#include <string>
#include <iterator>

#include "Client.hpp"
#include "Define.hpp"

#define OPER    1
#define NORMAL  2
#define SIZE    3

class Channel {
    private:
        std::map<const Client&, int>                    _users;
        std::map<const Client&, int>::iterator          _it;
        std::map<const Client&, int>::const_iterator    _coit;
        std::string                                     _name;

    public:
        Channel(const Client& client, std::string name);

        const std::string   getName(void) const;
        bool                checkAuth(const Client& client) const;
        bool                checkClient(std::string nick);
        void                addUser(const Client& client);
        void                delUser(const Client& client);
        int                 delByNick(std::string nick);
        int                 getUserSize(void) const;
        std::string         getUsersNames(void);
        std::vector<int>    getFds(int senderFd) const;
        void                opUser(std::string nick);
        void                deopUser(std::string nick);
};

#endif