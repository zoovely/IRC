#include <iostream>
#include <string>
#include <vector>

int main(void) {
    // std::string _msg = "CAP LS\nPASS 1234\nNICK lampolar\nUSER lampolar lampolar 127.0.0.1 :김희선";
	std::string _msg = "PRIVMSG #channel :fjekaw fejka fejka\r\n";
    std::vector<std::string> _splitMsg;
	std::string::size_type pos = _msg.find(" " , 0);
	std::string::size_type newpos = _msg.find("\r\n", 0);
	std::string::size_type colone = _msg.find(":", 0);
	int i = 0;
	while ((pos != std::string::npos || newpos != std::string::npos) && pos < colone)
	{
		if (pos > newpos)
		{
			_splitMsg.push_back(_msg.substr(i, newpos - i));
			i = newpos + 2;
		}
		else {
			_splitMsg.push_back(_msg.substr(i, pos - i));
			i = pos + 1;
		}
		pos = _msg.find(" ", i);
		newpos = _msg.find("\r\n", i);
	}
	if (colone != std::string::npos)
		_splitMsg.push_back(_msg.substr(colone, _msg.length() - colone -2));
	for (int i = 0; i < _splitMsg.size(); i++)
		std::cout << i << _splitMsg[i] << std::endl;
}
