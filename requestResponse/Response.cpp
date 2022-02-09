#include "Response.hpp"

Response::Response(){
}

Response::Response(Request requestClient, std::vector<Server> configParsed): statusCode(-1), location(), stringJoinedResponse(std::string()), \
clientRequest(requestClient), serverConfigData(configParsed){
}

Response::~Response(){
}

Server	*Response::findVirtualServer()
{
	int	defaultServerIndex = 0;
	std::vector<std::string> hostPort = ft_splitSpace(((clientRequest.getHttpHeaders()).find("Host"))->second, ':');
	if (hostPort.size() == 1)
		hostPort.push_back("80");
	for(int i = 0; i < serverConfigData.size(); i++)
	{
		if(!((serverConfigData[i]).get_listen()).compare(hostPort[1]))
		{
			if(i && ((serverConfigData[i - 1]).get_listen()) != ((serverConfigData[i]).get_listen()))
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
	size_t		pos = 0;
	for(std::map<std::string, Location>::iterator it = virtualServer->get_map_loc().begin(); it != virtualServer->get_map_loc().end(); it++)
	{
		if (!(it->first.compare("/")))
			this->location = it->second;
		if(!(clientRequest.getPath().compare(it->first))){
			this->location = it->second;
			return;
		}
	}
}

bool	Response::allowedMethods(){
	for (int i = 0; i < location.get_methods().size(); i++){
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
	return (statusCode);
}

int     Response::buildResponse()
{
	struct stat buf;
	this->virtualServer = this->findVirtualServer();
	this->filePath = virtualServer->get_root() + clientRequest.getPath();
	this->findLocation();
	stat(filePath.c_str(), &buf);
	if (this->allowedMethods())
	{
		//FIXME http://localhost:8080/return/
		if (accessFile(filePath))
		{
			if(!S_ISDIR(buf.st_mode))     // It is a file;
				return (returnStatus(OK, std::string("OK")));
			else
			{
				if (filePath.back() != '/')
				{
					redirection = clientRequest.getPath() + std::string("/");
					filePath = virtualServer->get_root() + "/movedPermanently.html";
					return (returnStatus(MOVEDPERMANENTLY, std::string("Moved Permanently")));
				}
				else
				{
						for(int i = 0; i < this->location.get_index().size(); i++)
						{
							if (accessFile(filePath + '/' + location.get_index().at(i)))
							{
								filePath += location.get_index().at(i);
								return (returnStatus(OK, std::string("OK")));
							}
						}
						if(!location.get_autoindex().compare("on"))
						{
							std::cout << "autoindex on need to create the approp webpage!!!" << std::endl;
							exit(0);
						}
					filePath = virtualServer->get_root() + "/notFound.html";
					return (returnStatus(NOTFOUND, "NOT FOUND"));
				}
			}
		}
		else
		{
			filePath = virtualServer->get_root() + "/notFound.html";
			return (returnStatus(NOTFOUND, "NOT FOUND"));
		}
	}
	else
	{
		filePath = virtualServer->get_root() + "/forbidden.html";
		return (returnStatus(FORBIDDEN, std::string("FORBIDDEN")));
	}
	return (0);
}

std::string	&Response::returnResponse(){
	buildResponse();
	return indexFound();
}


std::string &Response::indexFound(){
	std::ifstream	indexFile;
	std::string		str;
	std::string		htmlString;
	// std::cout << " ****** " << filePath << " ****** " << std::endl;
	indexFile.open(filePath);
	stringJoinedResponse += clientRequest.getHttpVersion() + " "; 
	stringJoinedResponse += std::to_string(statusCode) + " ";
	stringJoinedResponse += statusMessage + " \r\n";
	while(std::getline(indexFile, str))
		htmlString += str;
	stringJoinedResponse += "Content-Length: ";
	stringJoinedResponse += std::to_string(htmlString.length());
	stringJoinedResponse += "\r\n";
	// if (statusCode == MOVEDPERMANENTLY)
	// {
		stringJoinedResponse += "Location: ";
		stringJoinedResponse +=	redirection; 
		stringJoinedResponse +=	" \r\n"; 
	// }	
	stringJoinedResponse += "Connection: close\r\n";
	stringJoinedResponse += "Content-Type: text/html\r\n\r\n";
	stringJoinedResponse += htmlString;
	return stringJoinedResponse;
}

// std::string Response::indexNotFound(){
// 	std::ifstream	indexFile;
// 	std::string		str;
// 	std::string		htmlString;
// 	indexFile.open("requestResponse/notFound.html");
// 	stringJoinedResponse += (clientRequest.getRequestLine())[2]; 
// 	stringJoinedResponse += " 404 NOTFOUND \n";
// 	while(std::getline(indexFile, str))
// 		htmlString += str;
// 	stringJoinedResponse += "Content-Length: ";
// 	stringJoinedResponse += std::to_string(htmlString.length());
// 	stringJoinedResponse += "\n";
// 	stringJoinedResponse += "Connection: close\n";
// 	stringJoinedResponse += "Content-Type: text/html\n\n";
// 	stringJoinedResponse += htmlString;
// 	return stringJoinedResponse;
// }

// std::string Response::indexForbidden(){
// 	std::ifstream	indexFile;
// 	std::string		str;
// 	std::string		htmlString;
// 	indexFile.open("requestResponse/forbidden.html");
// 	stringJoinedResponse += (clientRequest.getRequestLine())[2]; 
// 	stringJoinedResponse += " 403 Forbidden \n";
// 	while(std::getline(indexFile, str))
// 		htmlString += str;
// 	stringJoinedResponse += "Content-Length: ";
// 	stringJoinedResponse += std::to_string(htmlString.length());
// 	stringJoinedResponse += "\n";
// 	stringJoinedResponse += "Connection: close\n";
// 	stringJoinedResponse += "Content-Type: text/html\n\n";
// 	stringJoinedResponse += htmlString;
// 	return stringJoinedResponse;
// }

// std::string Response::indexMovedPermanently(){
// 	std::ifstream	indexFile;
// 	std::string		str;
// 	std::string		htmlString;
// 	indexFile.open("requestResponse/movedPermanently.html");
// 	stringJoinedResponse += (clientRequest.getRequestLine())[2]; 
// 	stringJoinedResponse += " 301 Moved Permanently \n";
// 	while(std::getline(indexFile, str))
// 		htmlString += str;
// 	stringJoinedResponse += "Content-Length: ";
// 	stringJoinedResponse += std::to_string(htmlString.length());
// 	stringJoinedResponse += "\n";
// 	stringJoinedResponse += "Connection: keep-alive\n";
// 	stringJoinedResponse += "Content-Type: text/html\n";
// 	stringJoinedResponse += "Location: redirect.html\r\n\r\n";
// 	stringJoinedResponse += htmlString;
// 	return stringJoinedResponse;
// }



