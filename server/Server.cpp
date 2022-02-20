/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/20 15:46:22 by ybouddou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../WebServ.hpp"

int		SetupSocket(int port)
{
	struct sockaddr_in	servadd;
	int					sockfd;
	int					flag = 1;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servadd, sizeof(servadd));
	servadd.sin_family = AF_INET;
	// if (host.empty())
	// 	host = "127.0.0.1";
	// servadd.sin_addr.s_addr = inet_addr(host.c_str());
	servadd.sin_addr.s_addr = htonl(INADDR_ANY);
	servadd.sin_port = htons(port);
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
		throw std::runtime_error("setsockopt failed");
	if (bind(sockfd, (const struct sockaddr *) &servadd, sizeof(servadd)) < 0)
		throw std::runtime_error("bind failed");
	if (listen(sockfd, 0) < 0)
		throw std::runtime_error("listen failed");
	getsockname(sockfd, (struct sockaddr *) &servadd, (socklen_t *)sizeof(servadd));
	std::cout << "\e[1;32m\u2718 Listening on : \e[0;36m" << inet_ntoa(servadd.sin_addr) << ":" << ntohs(servadd.sin_port) << "\e[0m\n"; 
	return (sockfd);
}

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
	std::cout << "\n\e[1;32m\u2714\e[1;34m Connection accepted from \e[0;36m" << client_address << ":" << client_port << "\e[0m\n\n";
	return (acceptfd);
}

void	recvRequest(t_WebServ *webserv)
{
	size_t	pos;
	
	webserv->client = (Client *)webserv->event.udata;
	bzero(webserv->client->receive, 1024);
	webserv->client->ret = recv(webserv->event.ident, webserv->client->receive, 1023, 0);
	webserv->client->req.append(webserv->client->receive, webserv->client->ret);
	if (webserv->client->header.empty())
	{
		if((pos = webserv->client->req.find("\r\n\r\n")) != std::string::npos)
		{
			webserv->client->header = webserv->client->req.substr(0, pos + 4);
			webserv->client->req.erase(0, pos + 4);
			webserv->client->lenght = 0;
			if((pos = webserv->client->header.find("Content-Length: ")) != std::string::npos)
			{
				std::string	tmp = webserv->client->header;
				tmp.erase(0, pos + 16);
				webserv->client->lenght = stoi(tmp.substr(0, tmp.find("\r\n")));
			}
		}
	}
	if (!webserv->client->header.empty() && webserv->client->req.size() == webserv->client->lenght)
	{
		std::cout << "\e[1;32m--> Request has been received successfully !!\e[0m\n\n";
		webserv->client->req = webserv->client->header + webserv->client->req;
		webserv->client->request = Request(webserv->client->req);
		webserv->client->req = "";
		EV_SET(&webserv->event, webserv->client->socket, EVFILT_READ, EV_DELETE, 0, 0, (void *)(webserv->client));
		kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		sendResponse(webserv);
		EV_SET(&webserv->event, webserv->client->socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, (void *)(webserv->client));
		kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
	}
}

void	sendResponse(t_WebServ *webserv)
{
	webserv->client = (Client *)webserv->event.udata;

	if (webserv->client->res.empty())
	{
		Response response(webserv->client->request, webserv->servers);
		webserv->client->res = response.returnResponse();
		webserv->client->size = webserv->client->res.size();
	}
	webserv->client->sent += send(webserv->event.ident, webserv->client->res.c_str() + webserv->client->sent, webserv->client->size - webserv->client->sent, 0);
	if ((webserv->client->size - webserv->client->sent) == 0)
	{
		std::cout << "\e[1;36m<-- Response has been sent successfully !!\e[0m\n\n";
		delete[] webserv->client;
		close(webserv->event.ident);
	}
}

void	multipleServers(t_WebServ *webserv)
{
	struct kevent	change;
	std::map<std::string, int>::iterator it;

	webserv->kq = kqueue();
	it = webserv->ports.begin();
	while (it != webserv->ports.end())
	{
		memset(&webserv->event, 0, sizeof(webserv->event));
		webserv->sockfd = SetupSocket(it->second);
		fcntl(webserv->sockfd, F_SETFL, O_NONBLOCK);
		EV_SET(&webserv->event, webserv->sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		webserv->sockets.push_back(webserv->sockfd);
		it++;
	}
	multipleClient(webserv);
}

bool	isServer(std::vector<int> sockets, int sockfd)
{
	std::vector<int>::iterator	it;

	it = sockets.begin();
	while (it < sockets.end())
	{
		if (sockfd == (*it))
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
		{
			std::cout << "\e[0;31mEOF received\e[0m\n";
			close(webserv->event.ident);
		}
		else if (isServer(webserv->sockets, webserv->event.ident))
		{
			webserv->acceptfd = accept_connection(webserv->event.ident);
			webserv->client = new Client[1];
			bzero(webserv->client, sizeof(Client));
			webserv->client->socket = webserv->acceptfd;
			EV_SET(&webserv->event, webserv->acceptfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void *)(webserv->client));
			kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		}
		else if (webserv->event.filter == -1)
			recvRequest(webserv);
		else if (webserv->event.filter == -2)
			sendResponse(webserv);
	}
}
