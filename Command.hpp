#ifndef COMMAND_H
#define COMMAND_H

#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>

#include "Client.hpp"
#include "Channel.hpp"
#include "Define.hpp"

#define CONNECT 1
#define JOIN    2
#define PART    3
#define INVITE  4
#define KICK    5
#define NICK    6
#define LIST    7
#define WHOIS   8
#define QUIT    9
#define PING    10
#define OP      11
#define DEOP    12
#define PRIVMSG 13
#define PRIVCH  14
#define NOTICE  15
#define NOTICH  16
#define MODE_I  17
#define MODE_N  18

class Command {
    private:
        std::string _msg;
        std::vector<std::string> _splitMsg;
        
    public:
        Command(std::string msg);
        void splitMsg(void);

        // define에대해서 확인 & 디파인값 확인 (없는거는 0 반환)
        int checkMsgType(void);
        // client fd로 메세지를 출력  // 파라미터 바뀌어야될듯
        void sendFd(int fd, std::string str);
        void sendAll(std::vector<int> fds, std::string str);
        int checkValidChannel(const std::string chName, const std::vector<Channel> &chList);
        int checkValidClient(const std::string nick, const std::vector<Client> &cList);
        int	checkValidNick(const std::string nick);
        void delChannel(std::vector <Channel> &chList, Channel &channel);
        std::string getUsers(const Channel &channel);

        //command list 
        int connect(int fd, std::string pwd, std::vector<Client> &cList); // 비번인증, 닉네임 중복검사 내부에서 필요& & welcome msg전송
        int join(const Client &client, std::vector<Channel> &chList);
        int part(const Client &client, std::vector<Channel> &chList); // channel에 남은 사람이 있는지 확인
        int invite(const Client &client, const std::vector<Channel> &chList, const std::vector<Client> &cList);
        int kick(const Client &client, std::vector<Channel> &chList); // channel에 남은 사람이 있는지 확인 
        int nick(Client &client, const std::vector<Client> &cList, const std::vector<Channel> &chList);
        int list(const Client &client, const std::vector<Channel> &chList);
        int whois(const Client &client, const std::vector<Client> &cList);
        int quit(Client &client, std::vector<Channel> &chList, std::vector<Client> &cList); // channel에 남은 사람이 있는지 확인 
        int ping(const Client &client);
        int op(const Client &client, std::vector<Channel> &chList);
        int deop(const Client &client, std::vector<Channel> &chList);
        int privmsg(const Client &sender, const std::vector<Channel> &chList);
        int privmsg(const Client &sender, const std::vector<Client> &cList);
        int notice(const Client &sender, const std::vector<Channel> &chList);
        int notice(const Client &sender, const std::vector<Client> &cList);
        int modeI(const Client &sender);
        int modeN(const Client &sender, const std::vector<Channel> &chList);
};

#endif