
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include "bircd.h"

void	client_read(t_env *e, int cs)
{
  int	r;
  int	i;

  r = recv(cs, e->fds[cs].buf_read, BUF_SIZE, 0);
  if (r <= 0)
    {
      close(cs);
      clean_fd(&e->fds[cs]);
      printf("client #%d gone away\n", cs);
    }
  else
    {
      i = 0;
      int j = 0;
      while (e->fds[cs].buf_read[j] != '\r')
      {
        j++;
      }
      printf("%s : %d\n", e->fds[cs].buf_read, j);
      while (i < e->maxfd)
      {
        if ((e->fds[i].type == FD_CLIENT) &&
            (i != cs))
          send(i, e->fds[cs].buf_read, r, 0);
        i++;
      }
    }
}

// CAP LS = 6  = 8
// NICK lampolar = 13 = 15
// USER lampolar lampolar 127.0.0.1 :김희선 = 37 = 39

// 56 62개 
// 12개가 부족해

// \r\n ㅁ

// savebuf[fd] X
// while 계속 읽는거 -> fd하나에read가 무한 와일
// vuf 4096 읽어
// 읽은 값 을 모두 savㅁebuf 에 저장해
// savebuf에서 \r\n
// \r\n 나눠서 커맨드 저장
// 실행시켜
// \r\n 안붙은애들 savebuf에 남겨

// std::string	Server::concatMessage(int clientSocket) {
// 	int			message_length;
// 	std::string	fullMsg = "";

// 	while ((message_length = recv(clientSocket, _message, BUF_SIZE, 0)) != 0) {
// 		if (message_length < 0) continue;
// 		_message[message_length] = 0;
// 		fullMsg += _message;
// 		if (fullMsg.length() >= 2 && !fullMsg.substr(fullMsg.length() - 2).compare("\r\n")) break;
// 	}
// 	ft_replaceStr(fullMsg, "\r", " ");

// 	return fullMsg;근데 cap ls \r\n을 기대하고 command를 쓴건데 
//   cap ls control+d pass 1234 control+d\r\n 이런식으로 작성하면
//    \r\n이 없어서 하나의 commmand로 쪼갤수 없는데 
//   그러면 server가 해당 nc에 대해서
//   제대로 명령어가 들어왔다고 인식을 못해서 아무것도 못해줄듯


/** 참고용
void	Server::serverOn(void) {
	pollFdIter	iter;

	if (listen(_serverSocket, 5) < 0)
		throw std::runtime_error(Error(ERR_SERVEROPENFAILED, "listen"));

	while (true) {
		if (!poll(_poll_fds.data(), _poll_fds.size(), 1000)) continue ;
		if (_poll_fds[0].revents & POLLIN) {
			acceptClient();
			continue;
		}
		for (iter = _poll_fds.begin() + 1; iter < _poll_fds.end(); iter++) {
			if (iter->revents & POLLHUP) { // 현재 클라이언트 연결 끊김
				stringVector	closeParams;
				User*	quitUser = findUser(iter->fd);

				closeParams.push_back(":Connection closed");
				quitUser->setIsSigquit(true);
				commandQUIT(quitUser, closeParams);
				continue ;
			}
			else if (iter->revents & POLLIN) {
				if (isServerUser(iter->fd))
					receiveClientMessage(iter->fd); // 이게 이미 열려있는 클라이언트 <- 여기서 저 concat 메시지 부름
				else
					receiveFirstClientMessage(iter->fd); // 이건 처음 들어오는 애 <- 여기서 저 concat 메시지 부름
			}
			if (iter->revents & POLLNVAL) _poll_fds.erase(iter);
		}
		disconnectClients();
		deleteChannel();
	}
}*/

}