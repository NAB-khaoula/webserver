/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:33:37 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/17 22:35:51 by ybouddou         ###   ########.fr       */
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
	int		socket;
	//recv
	Request	request;
	char	receive[1024];
	std::string		header;
	std::string		req;
	size_t	ret;
	size_t	lenght;
	
	//send
	char	*send;
	std::string	res;
	size_t	sent;
	size_t	rem;
	size_t	size;
}				Client;

typedef struct	ws
{
	std::vector<int>					sockets;
	std::vector<Server>					servers;
    Server								*serv;
    Location							locat;
	std::map<std::string, int>			ports;
	size_t								index;
	Client								*client;
	struct kevent						event;
	int									kq;
	int									nev;
	int									port;
	int									acceptfd;
	int									sockfd;
}				t_WebServ;

int				SetupSocket(int port);
int				accept_connection(int sockfd);
void			multipleServers(t_WebServ *webserv);
void			multipleClient(t_WebServ *webserv);
bool			isServer(std::vector<int> sockets, int sockfd);
void			recvRequest(t_WebServ *webserv);
void			sendResponse(t_WebServ *webserv);

#endif