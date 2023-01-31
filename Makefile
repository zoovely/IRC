CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
SRCS = main.cpp Server.cpp Client.cpp Channel.cpp Command.cpp 
OBJS = ${SRCS:.cpp=.o}
INCLUDES = Server.hpp Client.hpp Channel.hpp Command.hpp Define.hpp
NAME = ircserv

RM = rm -f

${NAME}:	${OBJS}
		${CXX} -o ${NAME} ${OBJS} ${CXXFLAGS}

all:		${NAME}

clean:
		${RM} ${OBJS} *~ #*#

fclean:		clean
		${RM} ${NAME}

re:		fclean all
