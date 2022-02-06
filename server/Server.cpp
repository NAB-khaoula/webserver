/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelaman <mbelaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/06 16:36:14 by mbelaman         ###   ########.fr       */
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
	// inet_ntop(AF_INET, &clientadd, client_address, 6000);
	// client_port = ntohs(clientadd.sin_port);
	// std::cout << "Accepted fd : " << acceptfd << " Connection from " << client_address << ", port " << client_port << "\n\n";
	return (acceptfd);
}

void	handle_connection(t_WebServ *webserv, struct kevent event)
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

void	multipleServers(t_WebServ *webserv)
{
	Sockets		sock;
	struct kevent	change;
	std::map<std::string, int>::iterator it;

	webserv->kq = kqueue();
	it = webserv->ports.begin();
	// std::cout  << webserv->it->get_ports().size() << "\n";
	while (it != webserv->ports.end())
	{
		memset(&webserv->event, 0, sizeof(webserv->event));
		sock.SetupSocket(it->second);
		std::cout << it->second << " | 127.0.0.1" << std::endl;
		EV_SET(&webserv->event, sock.getSockfd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		webserv->sockets.push_back(sock);
		it++;
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

void	multipleClient(t_WebServ *webserv)
{
	while (1)
	{
		webserv->nev = kevent(webserv->kq, NULL, 0, &webserv->event, 1, NULL);
		if (webserv->nev < 0)
			throw std::runtime_error("kevent");
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

// void	handle_connection(WebServ *webserv, int fd)
// {
// 	Request	request;
// 	char	*buffer = new char[1024];
// 	char	*response = new char[1024];

// 	memset(buffer, '\0', 1024);
// 	recv(fd, buffer, 1024, 0);
// 	request.parseRequest(buffer);
// 	std::cout << buffer << std::endl;
// 	Response resp(request, webserv->servers);
// 	delete[] buffer;
// 	// HTTP/1.1 200 OK\r\nContent-Length: 1024\nConnection: close\nContent-Type: text/html\n\nHelloooooo
// 	std::strcpy(response, (resp.buildResponse()).c_str());
// 	send(fd, response, strlen(response), 0);
// 	delete[] response;
// 	close(fd);
// }

// void	multipleServers(WebServ *webserv)
// {
// 	Sockets		sock;

// 	FD_ZERO(&webserv->current_set);
// 	webserv->it = webserv->servers.begin();
// 	while (webserv->it < webserv->servers.end())
// 	{
// 		webserv->port = stoi((*webserv->it).get_listen());
// 		sock.SetupSocket(webserv->port, (*webserv->it).get_host());
// 		FD_SET(sock.getSockfd(), &webserv->current_set);
// 		webserv->max_fd = sock.getSockfd();
// 		webserv->sockets.push_back(sock);
// 		webserv->it++;
// 	}
// 	multipleClient(webserv);
// }

// bool	isServer(std::vector<Sockets> sockets, int sockfd)
// {
// 	std::vector<Sockets>::iterator	it;

// 	it = sockets.begin();
// 	while (it < sockets.end())
// 	{
// 		if (sockfd == (*it).getSockfd())
// 			return (true);
// 		it++;
// 	}
// 	return (false);
// }

// void	multipleClient(WebServ *webserv)
// {
// 	int		i;

// 	while (1)
// 	{
// 		i = -1;
// 		webserv->ready_set = webserv->current_set;
// 		if (select(webserv->max_fd + 1, &webserv->ready_set, NULL, NULL, NULL) < 0)
// 			throw "Select";
// 		while(++i <= webserv->max_fd)
// 		{
// 			if (FD_ISSET(i, &webserv->ready_set))
// 			{
// 				if (isServer(webserv->sockets, i))
// 				{
// 					webserv->acceptfd = accept_connection(i);
// 					FD_SET(webserv->acceptfd, &webserv->current_set);
// 					if (webserv->acceptfd > webserv->max_fd)
// 						webserv->max_fd = webserv->acceptfd;
// 				}
// 				else
// 				{
// 					handle_connection(webserv, i);
// 					FD_CLR(i, &webserv->current_set);
// 				}
// 			}
// 		}
// 	}
// }
