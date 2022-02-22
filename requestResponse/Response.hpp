#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <filesystem>
#include <sstream>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <ctime>
#include <fstream>
#include "../parsing/Server.hpp"
#include "Request.hpp"

#define	OK					200
#define CREATED				201 				
#define MOVEDPERMANENTLY 	301
#define	NOTMODIFIED			304
#define	BADREQUEST			400
#define FORBIDDEN			403
#define	NOTFOUND 			404
#define METHODNOTALLOWED	405
#define INTERNALSERVERERROR	500

class Server;
class Request;

class Response
{
	private:
		std::string					cgiString;
		std::string					cgiHeaders;
		int							statusCode;
		std::string					statusMessage;
		std::string					filePath;
		std::string					redirection;
		Location					location;
		std::string					stringJoinedResponse;
		Request						clientRequest;
		std::vector<Server>			serverConfigData;
		Server						*virtualServer;				
	public:
		Response();
		Response(Request requestClient,std::vector<Server> configParsed);
		~Response();
		//NOTE: getters for all attributes;
		//NOTE: 
		void		setCgiHeaders(std::string cgi_headers);
		std::string	&getCgiHeaders();
		std::string	&getCgiString();
		int			&getStatusCode();
		std::string	&getStatusMessage();
		std::string	&get_filePath();
		std::string	&getRedirection();
		Location	&get_location();
		Request		&getClientRequest();
		Server		*getServer();
		bool		badRequest();
		int			buildResponse();
		Server		*findVirtualServer();
		void		findLocation();
		bool		findUploadLocation();
		bool		allowedMethods();
		bool		accessFile(std::string filename);
		std::string	&returnResponse();
		int			returnStatus(int, std::string);
		std::string	&indexFound();
		// std::string	indexNotFound();
		// std::string	indexForbidden();
		// std::string	indexMovedPermanently();
};

#endif