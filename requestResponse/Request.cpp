#include "Request.hpp"

Request::Request(): upload(false), requestLine(std::vector<std::string>()), httpHeaders(std::map<std::string, std::string>()){}

Request::~Request(){}

std::vector<std::string>    Request::ft_splitCrlf(std::string &str, const std::string &c)
{
    std::vector<std::string> words;

    size_t pos = 0;
    while ((pos = str.find(c)) != std::string::npos)
    {
        words.push_back(str.substr(0, pos));
        str.erase(0, pos + c.size());
		if (c == "\r\n\r\n")
			break ;
    }
    words.push_back(str);
    return words;
}

int		&Request::getContentLength()
{
	return (contentLength);
}

std::string		&Request::getContentType()
{
	return (contentType);
}

void		Request::setContentLength()
{
	std::map<std::string, std::string>::iterator i;
	size_t	pos;

	i = httpHeaders.find("Content-Length");
	if (i != httpHeaders.end())
		contentLength = stoi(i->second);
}

void		Request::setContentType()
{
	std::map<std::string, std::string>::iterator i;
	size_t	pos;

	i = httpHeaders.find("Content-Type");
	if (i != httpHeaders.end())
		contentType = i->second.substr(i->second.find(":") + 1);
	if ((pos = contentType.find("; boundary=")) != std::string::npos)
	{
		boundary = "--" + contentType.substr(pos + 11);
		contentType.erase(pos);
	}
}

void		Request::setFormData(std::string& req, std::string type)
{
	std::vector<std::string>	bodyHeaders;
	Body			body;
	size_t			pos;

	bzero(&body, sizeof(Body));
	req.erase(0, boundary.size() + 2);
	while ((pos = req.find(boundary)) != std::string::npos)
	{
		body.body = req.substr(0, pos - 2);
		bodyHeaders = ft_splitCrlf(ft_splitCrlf(body.body, "\r\n\r\n").at(0), "\r\n");
		body.ContentDispo = bodyHeaders.at(0).substr(bodyHeaders.at(0).find(":") + 2);
		body.ContentDispo.erase(9, body.ContentDispo.find("=") + 2 - 9);
		if (bodyHeaders.size() > 1)
		{
			body.name = body.ContentDispo.substr(9, body.ContentDispo.find(";") - 1 - 9);
			body.ContentDispo.erase(9, body.ContentDispo.find("=") + 2 - 9);
			body.fileName = body.ContentDispo.substr(9, body.ContentDispo.size() - 1 - 9);
			body.ContentDispo.erase(9);
			body.ContentType = bodyHeaders.at(1).substr(bodyHeaders.at(1).find(":") + 2);
		}
		else
		{
			body.name = body.ContentDispo.substr(9, body.ContentDispo.size() - 1 - 9);
			body.ContentDispo.erase(9);
		}
		bodies.push_back(body);
		bzero(&body, sizeof(Body));
		req.erase(0, pos + boundary.size() + 2);
	}
}

void		Request::parseBody(std::string req)
{
	Body			body;
	std::string		type;
	
	setContentType();
	setContentLength();
	if (!type.empty() && !boundary.empty())
		setFormData(req, type);
	else if (!type.empty())
	{
		body.body = req;
		bodies.push_back(body);
	}
	// ****Printing body****
	// std::ofstream	out_file("bodies.txt");
	// std::vector<Body>::iterator it = bodies.begin();
	// while (it != bodies.end())
	// {
	// 	if (type == "multipart/form-data")
	// 	{
	// 		// out_file << "| ContentDispo: " << it->ContentDispo << " |\n";
	// 		// out_file << "| ContentType : " << it->ContentType << " |\n";
	// 		// out_file << "| name : " << it->name << " |\n";
	// 		// out_file << "| fileName : " << it->fileName << " |\n";
	// 		std::ofstream	filename(it->fileName);
	// 		filename << it->body;
	// 	}
	// 	// out_file << "| Body : " << it->body << " |\n+++++++++++++++++++++++++++++++++++++\n";
	// 	it++;
	// }
	// // out_file.close();
}

bool	Request::uploadFile(){
	for(int i = 0; i < bodies.size(); i++)
	{
		if (!bodies.at(i).fileName.empty())
			return true;
	}
	return false;
}

Request::Request(std::string requestString)
{
	SplitFirstLine(requestString);
	this->SplitHeader(ft_splitCrlf(ft_splitCrlf(requestString, "\r\n\r\n").at(0), "\r\n"), ':');
	parseBody(requestString);
	upload = uploadFile();
}

bool			Request::getUpload(){
	return upload;
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

std::vector<Body>	&Request::getBodies(){
	return bodies;
}
