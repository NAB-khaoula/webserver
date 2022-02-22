/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelaman <mbelaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:33:37 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/22 13:23:45 by mbelaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/event.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <sys/ioctl.h>

#include "parsing/Server.hpp"
#include "requestResponse/Request.hpp"
#include "requestResponse/Response.hpp"

class Server;

typedef struct	Cl
{
	int				socket;
	//recv
	Request			request;
	char			receive[1024];
	std::string		header;
	std::string		req;
	size_t			ret;
	size_t			lenght;
	int				chunked;
	
	//send
	std::string		res;
	size_t			sent;
	size_t			size;
}				Client;

typedef struct	ws
{
	//parsing
    Server								*serv;
    Location							locat;
	
	//server
	std::vector<int>					sockets;
	std::vector<Server>					servers;
	std::map<std::string, int>			ports;
	Client								*client;
	struct sockaddr_in					servadd;
	struct kevent						event;
	int									kq;
	int									nev;
	int									port;
	int									acceptfd;
	int									sockfd;
}				t_WebServ;

void			SetupSocket(t_WebServ& webserv);
void			accept_connection(t_WebServ& webserv);
void			multipleServers(t_WebServ& webserv);
void			multipleClient(t_WebServ& webserv);
bool			isServer(t_WebServ& webserv);
void			recvRequest(t_WebServ& webserv);
void			sendResponse(t_WebServ& webserv);
void			unChunkRequest(std::string& req, int& chunk);
void			getReqHeader(Client *client);
void			toHex(std::string hex, int& length);

#endif