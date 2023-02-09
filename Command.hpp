#ifndef COMMAND_H
#define COMMAND_H

#include <unistd.h>
#include <sys/socket.h>
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
        int getClientByFd(int fd, std::vector<Client> cList);
        void delChannel(std::vector <Channel> &chList, Channel &channel);
        std::string getUsers(const Channel &channel);

        //command list 
        int connect(int fd, std::string pwd, std::vector<Client> &cList); // 비번인증, 닉네임 중복검사 내부에서 필요& & welcome msg전송
        int pass(Client &client, std::string pwd, std::vector<Client> &cList, int idx);
        int join(const Client &client, std::vector<Channel> &chList);
        int part(const Client &client, std::vector<Channel> &chList); // channel에 남은 사람이 있는지 확인
        int invite(const Client &client, const std::vector<Channel> &chList, const std::vector<Client> &cList);
        int kick(const Client &client, std::vector<Channel> &chList); // channel에 남은 사람이 있는지 확인 
        int nick(Client &client, std::vector<Client> &cList, const std::vector<Channel> &chList);
        int chkNick(std::string nickName, std::vector<Client> &cList, int fd);
        int user(Client &client, std::vector<Client> &cList);
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