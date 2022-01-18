#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <ctime>
#include <fstream>
#include "../parsing/Server.hpp"
#include "Request.hpp"

#define	OK					200
#define	NOTFOUND 			404
#define MOVEDPERMANENTLY 	301
#define FORBIDDEN			403

class Response
{
	private:
		int							statusCode;
		std::string					filePath;
		Location					location;
		std::string					stringJoinedResponse;
		Request						clientRequest;
		std::vector<Server>			serverConfigData;
		Server						virtualServer;				
	public:
		Response();
		Response(Request requestClient,std::vector<Server> configParsed);
		~Response();
		bool		compareStrings(std::string first, std::string seconde);
		int			findFileRequested();
		Server		findVirtualServer();
		Location	findLocation();
		bool		allowedMethods();
		bool		findFile(std::string &filename);
		std::string	&buildResponse();
		std::string	indexFound();
		std::string	indexNotFound();
		std::string	indexForbidden();
};

#endif