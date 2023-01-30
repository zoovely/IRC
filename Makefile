CXX = c++
CXXFLAGS = -I. -g3 -Wall -Werror -Wextra
SRCS =	main.cpp Server.cpp Client.cpp Channel.cpp Command.cpp 
OBJS = ${SRCS:.c=.o}
INCLUDES = Server.hpp Clien.hpp Channel.hpp
NAME = ircserv

RM = rm -f

${NAME}:	${OBJS}
		${CC} -o ${NAME} ${OBJS} ${LDFLAGS}

all:		${NAME}

clean:
		${RM} ${OBJS} *~ #*#

fclean:		clean
		${RM} ${NAME}

re:		fclean all
