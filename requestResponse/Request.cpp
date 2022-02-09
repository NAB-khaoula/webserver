#include "Request.hpp"

Request::Request(): requestLine(std::vector<std::string>()), httpHeaders(std::map<std::string, std::string>()){}

Request::~Request(){}

std::vector<std::string>    Request::ft_splitCrlf(std::string &str, const std::string &c)
{
    std::vector<std::string> words;

    size_t pos = 0;
    while ((pos = str.find(c)) != std::string::npos)
    {
        words.push_back(str.substr(0, pos));
        str.erase(0, pos + c.size());
    }
    words.push_back(str);
    return words;
}

Request    &Request::parseRequest(char *buffer)
{
	std::string		requestString(buffer);

	SplitFirstLine(requestString);
	this->SplitHeader(ft_splitCrlf(ft_splitCrlf(requestString, "\r\n\r\n").at(0), "\r\n"), ':');
	std::cout << requestString << std::endl;
	//NOTE printing Data;
	// for(int i = 0; i < requestLine.size(); i++)
	// 	std::cout << "|" << requestLine[i] << "|" << std::endl;
	// for(std::map<std::string, std::string>::iterator i = URLVariable.begin(); i != URLVariable.end(); i++)
	// {
	// 	std::cout << "|" << i->first << "|" << " ******* " << "|" << i->second << "|" << std::endl;
	// }
	// for(std::map<std::string, std::string>::iterator i = httpHeaders.begin(); i != httpHeaders.end(); i++)
	// {
	// 	std::cout << "|" << i->first << "|" << " & " << "|" << i->second << "|" << std::endl;
	// }
	return(*this);
}

void			Request::parseParam(std::string	&variableURL, size_t &pos)
{
	std::vector<std::string>	vars;
	std::string	tempURL;
	std::string	sec;
	int			i;

	tempURL = variableURL.substr(pos + 1);
	variableURL.erase(pos);
	while ((pos = tempURL.find('&')) !=  std::string::npos)
	{
		vars.push_back(tempURL.substr(0, pos));
		tempURL.erase(0, pos + 1);
	}
	vars.push_back(tempURL);
	i = -1;
	while (++i < vars.size())
	{
		sec = std::string();
		if ((pos = vars[i].find('=')) != std::string::npos)
			sec = vars[i].substr(pos + 1);
		URLVariable[vars[i].substr(0, pos)] = sec;
	}
}

void			Request::SplitFirstLine(std::string &requestString)
{
	size_t			pos = 0;
	std::string		strSplited;
	std::string		variableURL;
	pos = requestString.find('\r');
	strSplited = requestString.substr(0, pos);
	requestString.erase(0, pos + 2);
	while((pos = strSplited.find(' ')) != std::string::npos)
	{
		this->requestLine.push_back(strSplited.substr(0, pos));
		strSplited.erase(0, pos + 1);
	}
	this->requestLine.push_back(strSplited);
	if ((pos = requestLine[1].find('?')) != std::string::npos)
		this->parseParam(requestLine[1], pos);
}

void    Request::SplitHeader(std::vector<std::string> vect, char c)
{
    size_t pos = 0;
    for(int i = 0; i < vect.size(); i++)
		if((pos = vect[i].find(c)) != std::string::npos)
			this->httpHeaders.insert(std::make_pair<std::string, std::string>(vect[i].substr(0, pos), vect[i].substr(pos + 2)));
}


std::vector<std::string>	&Request::getRequestLine(){
	return requestLine;
}

std::string					&Request::getMethod(){
	return requestLine.at(METHOD);
}

std::string					&Request::getPath(){
	return requestLine.at(PATH);
}

std::string					&Request::getHttpVersion(){
	return requestLine.at(HTTPVERSION);
}

std::map<std::string, std::string>	&Request::getHttpHeaders(){
	return httpHeaders;
}

