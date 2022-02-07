#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <ctime>
#include <fstream>
#include "../parsing/Server.hpp"
#include "Request.hpp"

#define	OK					200
#define	NOTFOUND 			404
#define MOVEDPERMANENTLY 	301
#define FORBIDDEN			403

class Server;

class Response
{
	private:
		int							statusCode;
		std::string					statusMessage;
		std::string					filePath;
		Location					location;
		std::string					stringJoinedResponse;
		Request						clientRequest;
		std::vector<Server>			serverConfigData;
		Server						*virtualServer;				
	public:
		Response();
		Response(Request requestClient,std::vector<Server> configParsed);
		~Response();
		bool		compareStrings(std::string first, std::string seconde);
		int			findFileRequested();
		Server		*findVirtualServer();
		Location	findLocation();
		bool		allowedMethods();
		bool		findFile(std::string filename);
		std::string	&buildResponse();
		int			returnStatus(int, std::string);
		std::string	&indexFound();
		// std::string	indexNotFound();
		// std::string	indexForbidden();
		// std::string	indexMovedPermanently();
};

#endif