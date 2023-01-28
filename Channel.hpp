#ifndef CHANNEL_H
#define CHANNEL_H

#include "Client.hpp"
#include <vector>

#define OPER 1
#define NORMAL 2

#define PUBLIC 3
#define PRIVATE 4

class Channel {
    private:
        std::vector<std::pair<Client, int> > _users; // uesr와, 해당 채널에서의 권한
        std::vector<Client> _ban;
         //ban 당한 nickname들의 list string일 경우: 다시 들어올수 있지 않나?
         //private인지 public인지를 확인할 수 잇는 flag
        std::string _name;
        int _mode;

    public:
        Channel(Client c1, Client c2);
        Channel(std::string name);
        ~Channel(void){};
        //operator 아닐경우 싪패-> 1 리턴?
        //누가 addban 을 요청했는가
        std::string getName(void);
        bool checkAuth(Client client);
        int checkClient(std::string nickName);
        bool checkBan(std::string nickName);
        int join(Client client);
        int ban(Client client, std::string nickName);
        int unban(Client client, std::string nickName);
        int invite(Client client, std::string nickName);
        int kick(Client client, std::string nickName);
        int op(Client client, std::string nickName);
        int deop(Client client, std::string nickName);
};

#endif