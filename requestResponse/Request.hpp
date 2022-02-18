#ifndef REQUEST_HPP
# define REQUEST_HPP
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <fstream>

#define METHOD		0
#define PATH		1
#define HTTPVERSION	2

typedef struct	s_Body
{
	std::string		body;
	std::string		ContentDispo;
	std::string		ContentType;
	std::string		name;
	std::string		fileName;
}				Body;

class Request{
	protected:
		bool								upload;
		std::vector<std::string>			requestLine;
		std::map<std::string, std::string>	URLVariable;
		std::map<std::string, std::string>	httpHeaders;

		// Body variables
		std::vector<Body>					bodies;
		std::string							type;
		std::string							boundary;
	public:
		Request();
		Request(std::string);
		~Request();
		std::vector<std::string>			&getRequestLine();
		std::string							&getMethod();
		std::string							&getPath();
		std::vector<Body>					&getBodies();
		std::string							&getHttpVersion();
		std::map<std::string, std::string>	&getHttpHeaders();
		void								getContentType(std::string& type);
		bool								getUpload();
		std::vector<std::string>    		ft_splitCrlf(std::string &str,const std::string &c);
		Request								&parseRequest(char *);
		void								parseBody(std::string req);
		void								setFormData(std::string& req, std::string type);
		void								SplitHeader(std::vector<std::string> vect, char c);
		void								SplitFirstLine(std::string& requestString);
		void								parseParam(std::string	&variableURL, size_t &pos);
		bool								uploadFile();
};

#endif