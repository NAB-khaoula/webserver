#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>
#include <vector>
#include <string>

class Request{
	protected:
		std::vector<std::string>			startLine;
		std::map<std::string, std::string>	httpHeaders;		
	public:
	Request();
	~Request();
	std::vector<std::string>			&getStartLine();
	std::map<std::string, std::string>	&getHttpHeaders();
	std::vector<std::string>    		ft_splitCrlf(std::string str,const std::string &c);
	Request								&parseRequest(char *);
	std::map<std::string, std::string>	SplitHeader(std::vector<std::string> vect, char c);
	void								SplitFirstLine(std::string& requestString);
};

#endif