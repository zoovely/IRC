#include <iostream>
#include <string>
#include <vector>

int main(void) {
    std::string _msg = "CAP LS\nPASS 1234\nNICK lampolar\nUSER lampolar lampolar 127.0.0.1 :김희선";
    std::vector<std::string> _splitMsg;
		std::string::size_type pos = _msg.find(" " , 0);
	std::string::size_type newpos = _msg.find("\n", 0);
	int i = 0;
	while (pos != std::string::npos || newpos != std::string::npos)
	{
		if (pos > newpos)
			pos = newpos;
		_splitMsg.push_back(_msg.substr(i, pos - i));
		i = pos + 1;
		pos = _msg.find(" ", i);
		newpos = _msg.find("\n", i);
	}
	_splitMsg.push_back(_msg.substr(i, _msg.length() - i));
	for (int i = 0; i < _splitMsg.size(); i++)
		std::cout << i << _splitMsg[i] << std::endl;
}
