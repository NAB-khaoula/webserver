/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/01/18 16:37:04 by ybouddou         ###   ########.fr       */
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
	// HTTP/1.1 200 OK\r\nContent-Length: 1024\nConnection: close\nContent-Type: text/html\n\nHelloooooo
	std::strcpy(response, (resp.buildResponse()).c_str());
	// std::cout << response << std::endl;
	// std::strcpy(response, "HTTP/1.1 200 OK\r\nContent-Length: 10\nConnection: close\nContent-Type: text/html\n\nHelloooooo");
	send(event.ident, response, strlen(response), 0);
	delete[] response;
	close(event.ident);
}

void	multipleServers(WebServ *webserv)
{
	Sockets		sock;

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
