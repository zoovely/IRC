#ifndef COMMAND_H
#define COMMAND_H

#include <unistd.h>
#include <sys/socket.h>
#include <list>
#include <ctype.h>

#include "Client.hpp"
#include "Channel.hpp"
#include "Define.hpp"

#define CONNECT 1
#define PASS    2
#define JOIN    3
#define PART    4
#define INVITE  5
#define KICK    6
#define NICK    7
#define USER    8
#define LIST    9
#define WHOIS   10
#define QUIT    11
#define PING    12
#define OP      13
#define DEOP    14
#define PRIVMSG 15
#define PRIVCH  16
#define NOTICE  17
#define NOTICH  18
#define MODE_I  19
#define MODE_N  20

class Command {
    private:
        std::string _msg;
        std::vector<std::string> _splitMsg;
        std::list<Channel>::iterator _chit;
        std::list<Channel>::const_iterator _coChit;
        std::list<Client>::iterator _cit;
        std::list<Client>::const_iterator _coCit;
        
    public:
        Command(std::string msg);
        void splitMsg(void);
        int checkMsgType(void);
        void sendFd(int fd, std::string str);
        void sendAll(std::vector<int> fds, std::string str);
        std::list<Channel>::const_iterator checkValidChannel(const std::string chName, const std::list<Channel> &chList);
        std::list<Client>::const_iterator checkValidClient(const std::string nick, const std::list<Client> &cList);
        int	checkValidNick(const std::string nick);
      
        std::list<Client>::iterator getClientByFd(int fd, std::list<Client> cList);
        void delChannel(std::list<Channel> &chList, std::string name);
        std::string getUsers(const Channel &channel);
        void welcomeMsg(Client &client);

        //command list 
        int connect(int fd, std::string pwd, std::list<Client> &cList);
        int pass(Client &client, std::string pwd, std::list<Client> &cList, std::list<Client>::iterator cIt);
        int join(const Client &client, std::list<Channel> &chList);
        int part(const Client &client, std::list<Channel> &chList);
        int invite(const Client &client, const std::list<Channel> &chList, const std::list<Client> &cList);
        int kick(const Client &client, std::list<Channel> &chList);
        int nick(Client &client, std::list<Client> &cList, const std::list<Channel> &chList);
        int chkNick(std::string nickName, std::list<Client> &cList, int fd);
        int user(Client &client, std::list<Client> &cList);
        int list(const Client &client, const std::list<Channel> &chList);
        int whois(const Client &client, const std::list<Client> &cList);
        int quit(std::list<Client>::iterator cIt, std::list<Channel> &chList, std::list<Client> &cList);
        int ping(const Client &client);
        int op(const Client &client, std::list<Channel> &chList);
        int deop(const Client &client, std::list<Channel> &chList);
        int privmsg(const Client &sender, const std::list<Channel> &chList);
        int privmsg(const Client &sender, const std::list<Client> &cList);
        int notice(const Client &sender, const std::list<Channel> &chList);
        int notice(const Client &sender, const std::list<Client> &cList);
        int modeI(const Client &sender);
        int modeN(const Client &sender, const std::list<Channel> &chList);
};

#endif