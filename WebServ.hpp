/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelaman <mbelaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:33:37 by ybouddou          #+#    #+#             */
/*   Updated: 2022/01/15 20:31:15 by mbelaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/event.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "parsing/Server.hpp"
#include "server/Sockets.hpp"
#include "requestResponse/Request.hpp"
#include "requestResponse/Response.hpp"

typedef struct	WevServ
{
	std::vector<Sockets>	sockets;
	std::vector<Server>		servers;
	std::vector<Server>::iterator	it;
	struct kevent	event;
	int				kq;
	int				port;
	int				nev;
	int				acceptfd;
	int				sockfd;
}				WebServ;


void	multipleServers(WebServ *webserv);
void	multipleClient(WebServ *webserv);
bool	isServer(std::vector<Sockets> sockets, int sockfd);
int		accept_connection(int sockfd);
void	handle_connection(WebServ *webserv, struct kevent event);
