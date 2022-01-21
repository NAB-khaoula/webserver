/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/01/15 19:26:52 by ybouddou         ###   ########.fr       */
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

void	handle_connection(WebServ *webserv, int fd)
{
	Request	request;
	char	*buffer = new char[1024];
	char	*response = new char[1024];

	memset(buffer, '\0', 1024);
	recv(fd, buffer, 1024, 0);
	request.parseRequest(buffer);
	std::cout << buffer << std::endl;
	Response resp(request, webserv->servers);
	delete[] buffer;
	std::strcpy(response, (resp.buildResponse()).c_str());
	send(fd, response, strlen(response), 0);
	delete[] response;
	close(fd);
}

void	multipleServers(WebServ *webserv)
{
	Sockets		sock;

	FD_ZERO(&webserv->current_set);
	webserv->it = webserv->servers.begin();
	while (webserv->it < webserv->servers.end())
	{
		webserv->port = stoi((*webserv->it).get_listen());
		sock.SetupSocket(webserv->port, (*webserv->it).get_host());
		FD_SET(sock.getSockfd(), &webserv->current_set);
		webserv->max_fd = sock.getSockfd();
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
	int		i;

	while (1)
	{
		i = -1;
		webserv->ready_set = webserv->current_set;
		if (select(webserv->max_fd + 1, &webserv->ready_set, NULL, NULL, NULL) < 0)
			throw "Select";
		while(++i < webserv->max_fd)
		{
			if (FD_ISSET(i, &webserv->ready_set))
			{
				if (isServer(webserv->sockets, i))
				{
					webserv->acceptfd = accept_connection(i);
					FD_SET(webserv->acceptfd, &webserv->current_set);
					if (webserv->acceptfd > webserv->max_fd)
						webserv->max_fd = webserv->acceptfd;
				}
				else
				{
					handle_connection(webserv, i);
					FD_CLR(i, &webserv->current_set);
				}
			}
		}
	}
}

// void	handle_connection(WebServ *webserv, struct kevent event)
// {
// 	Request	request;
// 	char	*buffer = new char[event.data + 1];
// 	char	*response = new char[1024];

// 	recv(event.ident, buffer, event.data, 0);
// 	buffer[event.data] = '\0';
// 	request.parseRequest(buffer);
// 	std::cout << buffer << std::endl;
// 	Response resp(request, webserv->servers);
// 	delete[] buffer;
// 	std::strcpy(response, (resp.buildResponse()).c_str());
// 	send(event.ident, response, strlen(response), 0);
// 	delete[] response;
// 	close(event.ident);
// }

// void	multipleServers(WebServ *webserv)
// {
// 	Sockets		sock;
// 	struct kevent	change;

// 	webserv->kq = kqueue();
// 	webserv->it = webserv->servers.begin();
// 	while (webserv->it < webserv->servers.end())
// 	{
// 		// try
// 		// {
// 			memset(&webserv->event, 0, sizeof(webserv->event));
// 			webserv->port = stoi((*webserv->it).get_listen());
// 			sock.SetupSocket(webserv->port, (*webserv->it).get_host());
// 			std::cout << webserv->port << " | " << (*webserv->it).get_host() << std::endl;
// 			EV_SET(&webserv->event, sock.getSockfd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
// 			kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
// 			webserv->sockets.push_back(sock);
// 			webserv->it++;
// 		// }
// 		// catch(char const *e)
// 		// {
// 		// 	std::cout << e << std::endl;
// 		// 	break;
// 		// }
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
// 	while (1)
// 	{
// 		webserv->nev = kevent(webserv->kq, NULL, 0, &webserv->event, 1, NULL);
// 		if (webserv->nev < 0)
// 			throw "kevent";
// 		if (webserv->event.flags & EV_EOF)
// 			close(webserv->event.ident);
// 		else if (isServer(webserv->sockets, webserv->event.ident))
// 		{
// 			webserv->acceptfd = accept_connection(webserv->event.ident);
// 			EV_SET(&webserv->event, webserv->acceptfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
// 			kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
// 		}
// 		else if (webserv->event.filter & EVFILT_READ)
// 			handle_connection(webserv, webserv->event);
// 	}
// }
