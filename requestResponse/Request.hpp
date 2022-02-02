#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>
#include <vector>
#include <string>

#define METHOD		0
#define PATH		1
#define HTTPVERSION	2

class Request{
	protected:
		// adding file descriptor;
		std::vector<std::string>			requestLine;
		std::map<std::string, std::string>	URLVariable;
		std::map<std::string, std::string>	httpHeaders;
	public:
		Request();
		~Request();
		std::vector<std::string>			&getRequestLine();
		std::string							&getMethod();
		std::string							&getPath();
		std::string							&getHttpVersion();
		std::map<std::string, std::string>	&getHttpHeaders();
		std::vector<std::string>    		ft_splitCrlf(std::string &str,const std::string &c);
		Request								&parseRequest(char *);
		std::map<std::string, std::string>	SplitHeader(std::vector<std::string> vect, char c);
		void								SplitFirstLine(std::string& requestString);
		void								parseParam(std::string	&variableURL, size_t &pos);
};

#endif