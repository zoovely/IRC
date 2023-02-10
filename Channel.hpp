#ifndef CHANNEL_H
#define CHANNEL_H

#include <map>
#include <vector>
#include <iterator>
#include <string>

#include "Client.hpp"
#include "Define.hpp"

#define OPER 1
#define NORMAL 2

class Channel {
    private:
        std::map<const Client&, int> _users;

        std::map<const Client&, int>::iterator _it;
        std::map<const Client&, int>::const_iterator _coit;
        std::string _name;

    public:
        Channel(const Client& client, std::string name);
        std::vector<int> getFds(int senderFd) const;
        const std::string getName(void) const;
        bool checkAuth(const Client& client) const;
        bool checkClient(std::string nick);
        void addUser(const Client& client);
        void delUser(const Client& client);
        void delByNick(std::string nick);
        void opUser(std::string nick);
        void deopUser(std::string nick);
        int getUserSize(void) const;
        std::string getUsersNames(void);
};

#endif