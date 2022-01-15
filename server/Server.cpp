/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/01/15 19:17:12 by ybouddou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../WebServ.hpp"

int		accept_connection(int sockfd)
{
	int					acceptfd;
	struct sockaddr_in	clientadd;
	socklen_t			add_len;
	char				client_address[6000] = {0};
	uint16_t			client_port;

	acceptfd = accept(sockfd, (struct sockaddr *) &clientadd, &add_len);
	inet_ntop(AF_INET, &clientadd, client_address, 6000);
	client_port = ntohs(clientadd.sin_port);
	std::cout << "Accepted fd : " << acceptfd << " Connection from " << client_address << ", port " << client_port << "\n\n";
	return (acceptfd);
}

void	handle_connection(WebServ *webserv, struct kevent event)
{
	Request	request;
	char	*buffer = new char[event.data + 1];
	char	*response = new char[1024];

	recv(event.ident, buffer, event.data, 0);
	buffer[event.data] = '\0';
	request.parseRequest(buffer);
	std::cout << buffer << std::endl;
	Response resp(request, webserv->servers);
	delete[] buffer;
	std::strcpy(response, (resp.buildResponse()).c_str());
	send(event.ident, response, strlen(response), 0);
	delete[] response;
	close(event.ident);
}

void	multipleServers(WebServ *webserv)
{
	Sockets		sock;
	struct kevent	change;

	webserv->kq = kqueue();
	webserv->it = webserv->servers.begin();
	while (webserv->it < webserv->servers.end())
	{
		memset(&webserv->event, 0, sizeof(webserv->event));
		webserv->port = stoi((*webserv->it).get_listen());
		sock.SetupSocket(webserv->port, (*webserv->it).get_host());
		EV_SET(&webserv->event, sock.getSockfd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		webserv->sockets.push_back(sock);
		webserv->it++;
	}
	multipleClient(webserv);
}

bool	isServer(std::vector<Sockets> sockets, int sockfd)
{
	std::vector<Sockets>::iterator	it;

	it = sockets.begin();
	while (it < sockets.end())
	{
		if (sockfd == (*it).getSockfd())
			return (true);
		it++;
	}
	return (false);
}

void	multipleClient(WebServ *webserv)
{
	while (1)
	{
		webserv->nev = kevent(webserv->kq, NULL, 0, &webserv->event, 1, NULL);
		if (webserv->nev < 0)
			throw "kevent";
			if (webserv->event.flags & EV_EOF)
				close(webserv->event.ident);
			else if (isServer(webserv->sockets, webserv->event.ident))
			{
				webserv->acceptfd = accept_connection(webserv->event.ident);
				EV_SET(&webserv->event, webserv->acceptfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
				kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
			}
			else if (webserv->event.filter & EVFILT_READ)
				handle_connection(webserv, webserv->event);
	}
}

// void	multipleClient(int sockfd)
// {
// 	int		acceptfd;
// 	int		i;
// 	int		max_socket;
	
// 	fd_set	current_sockets;
// 	fd_set	ready_sockets;
// 	FD_ZERO(&current_sockets);
// 	FD_SET(sockfd, &current_sockets);
// 	max_socket = sockfd;
// 	while (1)
// 	{
// 		ready_sockets = current_sockets;
// 		std::cout << "    \e[1;32mWaiting for requests ...\e[0m \n\n";
// 		if ((select(max_socket + 1, &ready_sockets, NULL, NULL, NULL)) < 0)
// 			throw "select failed";
// 		i = -1;
// 		while (++i <= max_socket)
// 		{
// 			if (FD_ISSET(i, &current_sockets))
// 			{
// 				if (i == sockfd)
// 				{
// 					acceptfd = accept_connection(sockfd);
// 					FD_SET(acceptfd, &current_sockets);
// 					if (acceptfd > max_socket)
// 						max_socket = acceptfd;
// 				}
// 				else
// 				{
// 					handle_connection(i);
// 					FD_CLR(i, &current_sockets);
// 				}
// 			}
// 		}
// 	}
// }
