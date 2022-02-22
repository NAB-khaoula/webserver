#include "Response.hpp"

Response::Response(){
}

Response::Response(Request requestClient, std::vector<Server> configParsed): cgiString(), cgiHeaders(),statusCode(-1), location(), stringJoinedResponse(std::string()), \
clientRequest(requestClient), serverConfigData(configParsed){
}

std::string	&Response::getCgiString(){
	return	cgiString;
}

int			&Response::getStatusCode(){
	return statusCode;
}

std::string	&Response::getStatusMessage(){
	return statusMessage;
}

std::string	&Response::getCgiHeaders(){
	return cgiHeaders;
}

void	Response::setCgiHeaders(std::string cgi_headers){
	cgiHeaders = cgi_headers;
}

void	Response::setStatusCode(int status){
	statusCode = status;
}
void	Response::setStatusMesssage(std::string status){
	statusMessage = status;
}

std::string	&Response::get_filePath(){
	return filePath;
}

std::string	&Response::getRedirection(){
	return redirection;
}

Location	&Response::get_location(){
	return location;
}

Request		&Response::getClientRequest(){
	return clientRequest;
}

Server		*Response::getServer(){
	return virtualServer;
}

Response::~Response(){
}

Server	*Response::findVirtualServer()
{
	int	defaultServerIndex = 0;
	std::vector<std::string> hostPort = ft_splitSpace(((clientRequest.getHttpHeaders()).find("Host"))->second, ':');
	if (hostPort.size() == 1)
		hostPort.push_back("80");
	for(size_t i = 0; i < serverConfigData.size(); i++)
	{
		if(!((serverConfigData[i]).get_listen()).compare(hostPort[1]))
		{
			if(serverConfigData.at(i).get_df())
				defaultServerIndex = i;
			if(!((serverConfigData[i]).get_host()).compare(hostPort[0]))
					return (new Server(serverConfigData[i]));
			for (std::map<std::string, std::string>::iterator it = (serverConfigData[i].get_server_names()).begin(); it != (serverConfigData[i].get_server_names()).end(); it++)
				if(!(it->first.compare(hostPort[0])))
					return (new Server(serverConfigData[i]));
		}
	}
	return (new Server(serverConfigData[defaultServerIndex]));
}

void	Response::findLocation(){
	std::string	tempString (clientRequest.getPath());
	struct stat buf;
	size_t pos;
	stat(tempString.c_str(), &buf);
	if(!S_ISDIR(buf.st_mode))
	{
		if((pos = tempString.find_last_of('/')) != std::string::npos)
			tempString.erase(pos);
	}
	for(std::map<std::string, Location>::iterator it = virtualServer->get_map_loc().begin(); it != virtualServer->get_map_loc().end(); it++)
	{
		if (!(it->first.compare("/")))
			this->location = it->second;
		if(!(tempString.compare(it->first)) || !(tempString.compare(it->first + "/"))){
			this->location = it->second;
			return;
		}
	}
}

bool	Response::allowedMethods(){
	if (location.get_methods().empty())
		return false;
	for (size_t i = 0; i < location.get_methods().size(); i++){
		if (!(location.get_methods().at(i).compare(clientRequest.getMethod())))
			return true;
	}
	return false;
}

bool	Response::accessFile(std::string filename){
	return (!access(filename.c_str(), F_OK) && !access(filename.c_str(), R_OK) && !access(filename.c_str(), W_OK));
}

int		Response::returnStatus(int status_code, std::string status_message){
	statusMessage = status_message;
	statusCode = status_code;
	if (statusCode != OK)
	{
		if (statusCode == CREATED)
		{
			for(size_t i = 0; i < this->location.get_index().size(); i++)
			{
				if (accessFile(filePath + '/' + location.get_index().at(i)))
				{
					filePath = filePath + '/' + location.get_index().at(i);
				}
			}
		}
		else
		{
			for(std::map<int, std::string>::iterator it = virtualServer->get_err_pages().begin(); it != virtualServer->get_err_pages().end(); it++)
			{
				if (it->first == statusCode)
				{
					filePath = virtualServer->get_root() + it->second;
					return (statusCode);
				}
			}
			filePath = virtualServer->get_root() + "/errors/" + std::to_string(statusCode) + ".html";
		}
	}
	return (statusCode);
}

bool	Response::badRequest()
{
	char acceptedRequest[][17] = {"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH", "COPY", "LINK", "UNLINK", "PURGE", "LOCK", "UNLOCK", "PROPFIND", "VIEW"};
	for(int i = 0; i < 17; i++)
	{
		if (acceptedRequest[i] == clientRequest.getMethod())
			break;
		else if (i == 8)
			return true;
	}
	if (clientRequest.getHttpVersion() != "HTTP/1.0" && clientRequest.getHttpVersion() != "HTTP/1.1")
		return true;
	if (clientRequest.getRequestLine().size() != 3)
		return true;
	if (clientRequest.getHttpHeaders().find("Host") == clientRequest.getHttpHeaders().end())
		return true;
	return false;
}

int     Response::buildResponse()
{
	struct stat buf;
	this->virtualServer = this->findVirtualServer();
	this->findLocation();
	this->filePath = virtualServer->get_root() + clientRequest.getPath();
	std::cout << clientRequest.getPath() << "\n";
	stat(filePath.c_str(), &buf);
	if (clientRequest.getContentLength() > stoi(virtualServer->get_client_max_body()))
		return(returnStatus(PAYLOADTOOLARGE, "Payload Too Large"));
	if (badRequest())
		return (returnStatus(BADREQUEST, std::string("Bad Request")));
	if(this->clientRequest.getHttpHeaders().find("If-None-Match") != this->clientRequest.getHttpHeaders().end())
		return (returnStatus(NOTMODIFIED, "Not Modified"));
	if (this->allowedMethods())
	{
		if (accessFile(filePath))
		{
			if (!location.get_return().empty())
			{
				if (location.get_return().find(301) == location.get_return().end())
					return (returnStatus(FORBIDDEN, std::string("Forbidden")));
				redirection = location.get_return()[301];
				return (returnStatus(MOVEDPERMANENTLY, "Moved Permanently"));
			}
			if(!S_ISDIR(buf.st_mode))
			{
				statusCode = OK;
				statusMessage = "OK";
				if(filePath.find(".py") != std::string::npos || filePath.find(".php") != std::string::npos)
				{
					try{
						cgiString = runCgi(*this);
					}
					catch(std::exception e){
						return (returnStatus(INTERNALSERVERERROR, "Internal Server Error"));
					}
				}
				return (returnStatus(statusCode, statusMessage));
			}
			else
			{
				if (!clientRequest.getMethod().compare("POST") || !clientRequest.getMethod().compare("DELETE"))
				{
					statusCode = OK;
					statusMessage = "OK";
					try{
						cgiString = runCgi(*this);
					}
					catch(std::exception e){
						return (returnStatus(INTERNALSERVERERROR, "Internal Server Error"));
					}
				}
				else
				{
					if (filePath.back() != '/')
					{
						std::cout << "hna redirection\n";
						redirection = clientRequest.getPath() + std::string("/");
						return (returnStatus(MOVEDPERMANENTLY, std::string("Moved Permanently")));
					}
					else
					{
						if (!location.get_index().empty())
						{
							std::cout << "location " << this->location.get_path() << "\n";
							for(size_t i = 0; i < this->location.get_index().size(); i++)
							{
								std::cout << "dkhlt hna " << this->location.get_index()[i] << "\n";
								if (accessFile(filePath + '/' + location.get_index().at(i)))
								{
									filePath = filePath + '/' + location.get_index().at(i);
									statusCode = OK;
									statusMessage = "OK";
									if(filePath.find(".py") != std::string::npos || filePath.find(".php") != std::string::npos)
									{
										try{
											cgiString = runCgi(*this);
										}
										catch(std::exception e)
										{
											return (returnStatus(INTERNALSERVERERROR, "Internal Server Error"));
										}
										break;
									}
									return (returnStatus(statusCode, std::string(statusMessage)));
								}
							}
						}
						else if(!location.get_autoindex().compare("on"))
						{
							DIR *dir;
							struct dirent *ent;
							if ((dir = opendir (filePath.c_str())) != NULL) {
								cgiString += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>Index Table</title>\n</head>\n<body><center><h1>Index Table</h1></center>\n<hr>\n</body>\n</html>";
								while ((ent = readdir (dir)) != NULL)
								{
									cgiString += "<a href=\"/";
									cgiString += ent->d_name;
									cgiString += "\">";
									cgiString += ent->d_name;
									cgiString += "</a>";
									cgiString += "<br>";
							  	}
								cgiString += "</body>\n</html>\n";
							  closedir (dir);
							}
							return(returnStatus(OK, "OK"));
						}
						return (returnStatus(NOTFOUND, "NOT FOUND"));
					}
					
				}
				return (returnStatus(statusCode, statusMessage));
			}
		}
		else if (access(filePath.c_str(), F_OK))
			return (returnStatus(NOTFOUND, "NOTFOUND"));
		else
			return (returnStatus(FORBIDDEN, "FORBIDDEN"));
	}
	else if (!this->location.get_match())
	{
		return (returnStatus(FORBIDDEN, "FORBIDDEN"));
	}
	else
		return (returnStatus(METHODNOTALLOWED, std::string("METHOD NOT ALLOWED")));
}

std::string	&Response::returnResponse(){
	buildResponse();
	return indexFound();
}

std::string	DateGMT(){
	time_t rawtime;
    struct tm * timeinfo;
	std::string date;
	static const char wday_name[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char mon_name[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    time( &rawtime );
    timeinfo = localtime( &rawtime );
	date +=  wday_name[timeinfo->tm_wday]; 
	date += std::string(", ");
	date += std::to_string(timeinfo->tm_mday);
	date += std::string(" ");
	date += std::string(mon_name[timeinfo->tm_mon]);
	date += std::string(" ");
	date += std::to_string(1900 + timeinfo->tm_year);
	date += std::string(" ");
	date += std::to_string(timeinfo->tm_hour - 1);
	date += std::string(":");
	date += std::to_string(timeinfo->tm_min);
	date += std::string(":");
	date += std::to_string(timeinfo->tm_sec);
	date += std::string(" GMT");
	return (date);
}

std::string &Response::indexFound(){
	std::ifstream	indexFile(filePath, std::ios::binary);
	std::ostringstream buffer;
	std::string		htmlString;

	if (cgiString.empty())
	{
		buffer << indexFile.rdbuf();
		htmlString = "\r\n\r\n";
		htmlString += buffer.str();
		cgiString = buffer.str();
	}
	else
		htmlString = "\r\n" + getCgiHeaders() + "\r\n\r\n" + cgiString;
	stringJoinedResponse += clientRequest.getHttpVersion() + " "; 
	stringJoinedResponse += std::to_string(statusCode) + " ";
	stringJoinedResponse += statusMessage + " \r\n";
	stringJoinedResponse += "Content-Length: ";
	stringJoinedResponse += std::to_string(cgiString.size());
	stringJoinedResponse += "\r\n";
	if (statusCode == MOVEDPERMANENTLY)
	{
		stringJoinedResponse += "Location: ";
		stringJoinedResponse +=	redirection; 
		stringJoinedResponse +=	" \r\n"; 
	}	
	if (statusCode == METHODNOTALLOWED)
	{
		stringJoinedResponse += "Allow: ";
		for(size_t i = 0; i < location.get_methods().size(); i++)
		{
			stringJoinedResponse += location.get_methods().at(i);
			if(i != location.get_methods().size() - 1)
				stringJoinedResponse += " , ";
		}
		stringJoinedResponse +=	" \r\n";
	}
	stringJoinedResponse += "Connection: ";
	stringJoinedResponse += clientRequest.getConnection();
	stringJoinedResponse +=	" \r\n";
	if(this->clientRequest.getHttpHeaders().find("Sec-Fetch-Dest")->second == std::string("style"))
		stringJoinedResponse +=  "Content-Type: text/css\r\n";
	else if (this->clientRequest.getHttpHeaders().find("Sec-Fetch-Dest")->second == std::string("script"))
		stringJoinedResponse +=  "Content-Type: text/javascript\r\n";
	// else if (this->clientRequest.getHttpHeaders().find("Sec-Fetch-Dest")->second == std::string("document"))
	// 	stringJoinedResponse +=  "Content-Type: text/html\r\n";
	else
		stringJoinedResponse += "Content-Type: */*\r\n";
	stringJoinedResponse += "Date: ";
	stringJoinedResponse += DateGMT();
	stringJoinedResponse += htmlString;
	std::ofstream resfile("response.txt", std::ios_base::app);
	resfile << stringJoinedResponse << std::endl;
	resfile.close();
	return stringJoinedResponse;
}
