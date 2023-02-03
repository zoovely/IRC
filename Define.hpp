#ifndef DEFINE_HPP
# define DEFINE_HPP

/* [WELCOME 001 - 002]*/
# define RPL_WELCOME(nick)							    ":LAZYIRC 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "\r\n"
# define RPL_YOURHOST(nick)							    ":LAZYIRC 002 " + nick + " :Your host is LAZYIRC, running version 1.0\r\n"
# define RPL_MODE_I(nick, ip)							":" + nick + "!" + nick + "@" + ip + " MODE " + nick + " :+i\r\n"

/* [JOIN 353 - 366] */
# define RPL_JOIN(nick, ip, channel)					":" + nick + "!" + nick + "@" + ip + " JOIN " + channel + "\r\n"
# define RPL_NAMREPLY(nick, channel, users)			    ":LAZYIRC 353 " + nick + " = " + channel + " :" + users + "\r\n"
# define RPL_ENDOFNAMES(nick, channel)                  ":LAZYIRC 366 " + nick + " " + channel + " :End of /NAMES list\r\n"
# define RPL_MODE_N(nick, channel)						":LAZYIRC 324 " + nick + " " + channel + " +n\r\n"

/* [PART] */ 
/* 채널에 전송 */
# define RPL_PART(nick, ip, channel)			    ":" + nick + "!" + nick + "@" + ip + " PART :" + channel + "\r\n"

/* [INVITE 341 - 345 ]*/
# define RPL_INVITING(nick, channel)				    ":LAZYIRC 341 " + nick + " " + channel + " :Inviting " + nick + " to " + channel + "\r\n"
# define RPL_INVITED(nick, channel)					    ":LAZYIRC 345 " + nick + " " + channel + " :You have been invited to " + channel + "\r\n"

/* [KICK]*/
# define RPL_KICK(nick,user, ip, channel, kickuser, msg)":" + nick + "!" + user + "@" + ip + " KICK " + channel + " " + kickuser + msg + "\r\n"

/* [NICK] */
# define RPL_NICK(nick, user, ip, newnick )			    ":" + nick + "!" + user + "@" + ip + " NICK :" + newnick + "\r\n"

/* [LIST 321 - 323] */
# define RPL_LISTSTART(nick)						    ":LAZYIRC 321 " + nick + " Channel : UsersNum\r\n"
# define RPL_LIST(nick, channel, numusers)	            ":LAZYIRC 322 " + nick + " " + channel + " " + numusers + "\r\n"
# define RPL_LISTEND(nick)							    ":LAZYIRC 323 " + nick + " :End of /LIST\r\n"

/* [WHOIS] */
# define RPL_WHOISUSER(nick, user, ip)				    ":LAZYIRC 311 nick "	 + nick + " ~" + nick + ip + " * :" + user + "\r\n"
# define RPL_WHOISSERVER					            ":LAZYIRC 312 * * LAZYIRC :Sponserd by WHERE42 team\r\n"
# define RPL_WHOISMODE(nick)							":LAZYIRC 379 " + nick + " " + nick + " :is using modes +i\r\n"
# define RPL_ENDOFWHOIS								    ":LAZYIRC 318\r\n"

/* [QUIT] */
# define RPL_QUIT(nick, user, ip, msg)					":" + nick + "!" + user + "@" + ip + " QUIT " + msg + "\r\n"

/* [PONG] */
# define RPL_PONG(ip)									":LAZYIRC PONG " + ip + "\r\n"

/* [OP DEOP] */
# define RPL_OP(nick, user, ip, channel, opuser)		":" + nick + "!" + user + "@" + ip + " MODE " + channel + " +o " + opuser + "\r\n";
# define RPL_DEOP(nick, user, ip, channel, deopuser)    ":" + nick + "!" + user + "@" + ip + " MODE " + channel + " -o " + deopuser + "\r\n";

/* [PRIVMSG] */
# define RPL_PRIVMSG(nick, user, ip, receive, msg)		":" + nick + "!" + user + "@" + ip + " PRIVMSG " + receive + " " + msg + "\r\n"

/* [NOTICE] */
# define RPL_NOTICE(nick, user, ip, receive, msg)		":" + nick + "!" + user + "@" + ip + " NOTICE " + receive + " " + msg + "\r\n"

/* [ERR_MSG 401 - 506]*/
# define ERR_NOSUCHNICK(nick, target)				    ":LAZYIRC 401 " + nick + " " + target + " :No such nick/channel\r\n"
# define ERR_NOSUCHSERVER(serv)						    ":LAZYIRC 402 " + serv + " :No such server\r\n"
# define ERR_NOSUCHCHANNEL(nick, channel)			    ":LAZYIRC 403 " + nick + " " + channel + " :No such channel\r\n"
# define ERR_CANNOTSENDTOCHAN(nick, channel)		    ":LAZYIRC 404 " + nick + " " + channel + " :Cannot send to channel\r\n"
# define ERR_NOORIGIN								    ":LAZYIRC 409 * :No origin specified\r\n"
# define ERR_NORECIPIENT(nick, command)				    ":LAZYIRC 411 " + nick + " :No recipient given " + command + "\r\n"
# define ERR_NOTEXTTOSEND(nick)						    ":LAZYIRC 412 " + nick + " :No text to send\r\n"
# define ERR_UNKNOWNCOMMAND							    ":LAZYIRC 421 * :Command not found\r\n"

# define ERR_NONICKNAMEGIVEN						    ":LAZYIRC 431 :No nickname given\r\n"
# define ERR_ERRONEUSNICKNAME(nick)					    ":LAZYIRC 432 " + nick + " :" + nick + " is Erroneus nickname\r\n"
# define ERR_NICKNAMEINUSE(nick)					    ":LAZYIRC 433 " + nick + " :" + nick + "\r\n"
# define ERR_NICKCOLLISION(nick)					    ":LAZYIRC 436 " + nick + " :Nickname collision KILL\r\n"
# define ERR_USERNOTINCHANNEL(nick, channel)            ":LAZYIRC 441 " + nick + " " + channel + " :They aren't on that channel\r\n"
# define ERR_NOTONCHANNEL(nick, channel)			    ":LAZYIRC 442 " + nick + " " + channel + " :You're not on that channel\r\n"
# define ERR_NOTREGISTERED							    ":LAZYIRC 451 * :You have not registered\r\n"

# define ERR_NEEDMOREPARAMS(nick, command)			    ":LAZYIRC 461 " + nick + " " + command + " :Not enough parameters\r\n"
# define ERR_ALREADYREGISTRED(nick)					    ":LAZYIRC 462 " + nick + " :You may not reregister\r\n"
# define ERR_PASSWDMISMATCH							    ":LAZYIRC 464 FAIL :Password incorrect\r\n"

# define ERR_CHANNELISFULL(nick, channel)			    ":LAZYIRC 471 " + nick + " " + channel + " :Cannot join channel (+l)\r\n"
# define ERR_UNKNOWNMODE(nick, flags)				    ":LAZYIRC 472 " + nick + " " +  flags + " :is unknown mode char to me\r\n"
# define ERR_NOPRIVILEGES(nick)						    ":LAZYIRC 481 " + nick + " :Permission Denied- You're not an IRC operator\r\n"
# define ERR_CHANOPRIVSNEEDED(nick, channel)		    ":LAZYIRC 482 " + nick + " " + channel + " :You're not channel operator\r\n"
# define ERR_NOOPERHOST(nick)						    ":LAZYIRC 491 " + nick + " :No O-lines for your host\r\n"
# define ERR_UMODEUNKNOWNFLAG(nick, channel)		    ":LAZYIRC 501 " + nick + " " + channel + " :Unknown MODE flag\r\n"
# define ERR_USERSDONTMATCH(nick)					    ":LAZYIRC 502 " + nick + " :Cant change mode for other users\r\n"
# define ERR_FLOOD(sec)								    ":LAZYIRC 504 * :flood detected, please wait " + sec + " seconds\r\n"

// # define ERR_FAILTOOPENFILE(nick, path)				    ":LAZYIRC 505 " + nick + " :Fail to open file" + path + "\r\n"
// # define ERR_NOSUCHFILE(nick, path)					    ":LAZYIRC 506 " + nick + " :No such file" + path + "\r\n"

// # define RPL_UMODEIS(nick, mode)					":LAZYIRC 221 " + nick + " " + mode + "\r\n"
// # define RPL_NONE(message)						":LAZYIRC 300 * :" + message + "\r\n"
// # define RPL_AWAY(nick, message)					":LAZYIRC 301 * " + nick + " :" + message + "\r\n"
// # define RPL_UNAWAY(nick)						":LAZYIRC 305 * " + nick + " :You are no longer marked as being away\r\n"
// # define RPL_NOWAWAY(nick)						":LAZYIRC 306 * " + nick + " :You have been marked as being away\r\n"

// # define RPL_MOTD(nick, comment) 				":LAZYIRC 372 " + nick + " :- " + comment + "\r\n"
// # define RPL_MOTDSTART(nick)						":LAZYIRC 375 " + nick + " :- IRC Message of the day - \r\n"
// # define RPL_ENDOFMOTD(nick)						":LAZYIRC 376 " + nick + " :End of MOTD command\r\n"
// # define RPL_YOUREOPER(nick)						":LAZYIRC 381 " + nick + " :You are now an IRC operator\r\n"

// # define RPL_DCCSEND(nick, file)					":LAZYIRC 346 " + nick + " " + file + " :DDC request sent from " + nick + "\r\n"
// # define RPL_DCCRECV(nick, file)					":LAZYIRC 347 " + nick + " " + file + " :DDC request received from " + nick + "\r\n"
// # define RPL_DCCSENT(nick, file)					":LAZYIRC 348 " + nick + " " + file + " :DDC request sent to " + nick + "\r\n"
// # define RPL_DCCRECVD(nick, file)				":LAZYIRC 349 " + nick + " " + file + " :DDC sent " + file + " for " + nick + "\r\n"

// # define RPL_CHANNELMODEIS(nick, channel)	        ":LAZYIRC 324 " + nick + " " + channel + " :+nt\r\n"
// # define RPL_CREATIONTIME(nick, channel, timestamp)	":LAZYIRC 329 " + nick + " " + channel + " :" + timestamp + "\r\n" 
// # define RPL_ENDOFBANLIST(nick, channel)			    ":LAZYIRC 368 " + nick + " " + channel + " :End of channel ban list\r\n"

// # define FORM_1(nick, user, cmd, channel)		":" + nick + "!" + user + "@" + ip + cmd + " :" + channel + "\r\n"
// # define FORM_2(nick, user, msg)					":" + nick + "!" + user + "@" + ip + msg + "\r\n"

#endif