/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/24 12:05:03 by ybouddou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../WebServ.hpp"

void	SetupSocket(t_WebServ& webserv)
{
	int					flag = 1;

	webserv.sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&webserv.servadd, sizeof(webserv.servadd));
	webserv.servadd.sin_family = AF_INET;
	webserv.servadd.sin_addr.s_addr = htonl(INADDR_ANY);
	webserv.servadd.sin_port = htons(webserv.port);
	
	if (setsockopt(webserv.sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
		throw std::runtime_error("setsockopt failed");
	if (bind(webserv.sockfd, (const struct sockaddr *) &webserv.servadd, sizeof(webserv.servadd)) < 0)
		throw std::runtime_error("bind failed");
	if (listen(webserv.sockfd, 0) < 0)
		throw std::runtime_error("listen failed");
	getsockname(webserv.sockfd, (struct sockaddr *) &webserv.servadd, (socklen_t *)sizeof(webserv.servadd));
	std::cout << "\e[1;32m\u2718 Listening on : \e[0;36m" << inet_ntoa(webserv.servadd.sin_addr) << ":" << ntohs(webserv.servadd.sin_port) << "\e[0m\n"; 
}

void	accept_connection(t_WebServ& webserv)
{
	char				client_address[13] = {0};
	socklen_t			add_len;
	uint16_t			client_port;

	webserv.acceptfd = accept(webserv.event.ident, (struct sockaddr *) &webserv.servadd, &add_len);
	inet_ntop(AF_INET, &webserv.servadd, client_address, 13);
	client_port = ntohs(webserv.servadd.sin_port);
	std::cout << "\n\e[1;32m\u2714\e[1;34m Connection accepted from \e[0;36m" << client_address << ":" << client_port << "\e[0m\n\n";
}

void	toHex(std::string hex, int& length)
{
	std::stringstream ss;

	ss << std::hex << hex;
	ss >> length;
}

void	unChunkRequest(std::string& req, int& chunk)
{
	std::string	unchunked;
	size_t	pos;
	int		length;

	while((pos = req.find("\r\n")) != std::string::npos)
	{
		length = 0;
		toHex(req.substr(0, pos), length);
		req.erase(0, pos + 2);
		unchunked += req.substr(0, length);
		req.erase(0, length + 2);
	}
	chunk = 2;
	req = unchunked;
}

void	getReqHeader(Client *client)
{
	size_t	pos;
	std::string	tmp;
	
	if (client->header.empty())
	{
		if((pos = client->req.find("\r\n\r\n")) != std::string::npos)
		{
			client->header = client->req.substr(0, pos + 4);
			client->req.erase(0, pos + 4);
			client->lenght = 0;
			if((pos = client->header.find("Transfer-Encoding: chunked\r\n")) != std::string::npos)
				client->chunked = 1;
			if((pos = client->header.find("Content-Length: ")) != std::string::npos)
			{
				tmp = client->header;
				tmp.erase(0, pos + 16);
				client->lenght = stoi(tmp.substr(0, tmp.find("\r\n")));
			}
		}
	}
}

void	recvRequest(t_WebServ& webserv)
{
	webserv.client = (Client *)webserv.event.udata;
	bzero(webserv.client->receive, 1024);
	webserv.client->ret = recv(webserv.event.ident, webserv.client->receive, 1023, 0);
	webserv.client->req.append(webserv.client->receive, webserv.client->ret);
	getReqHeader(webserv.client);
	if (webserv.client->chunked && webserv.client->req.find("0\r\n\r\n") != std::string::npos)
			unChunkRequest(webserv.client->req, webserv.client->chunked);
	if (!webserv.client->header.empty() && webserv.client->req.size() == webserv.client->lenght && webserv.client->chunked != 1)
	{
		std::cout << "\e[1;32m--> Request has been received successfully !!\e[0m\n\n";
		webserv.client->req = webserv.client->header + webserv.client->req;
		webserv.client->request = Request(webserv.client->req);
		webserv.client->req = "";
		EV_SET(&webserv.event, webserv.client->socket, EVFILT_READ, EV_DELETE, 0, 0, (void *)(webserv.client));
		kevent(webserv.kq, &webserv.event, 1, NULL, 0, NULL);
		sendResponse(webserv);
	}
}

void	sendResponse(t_WebServ& webserv)
{
	webserv.client = (Client *)webserv.event.udata;

	Response response;
	if (webserv.client->res.empty())
	{
		response = Response(webserv.client->request, webserv.servers);
		webserv.client->res = response.returnResponse();
		webserv.client->size = webserv.client->res.size();
		EV_SET(&webserv.event, webserv.client->socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, (void *)(webserv.client));
		kevent(webserv.kq, &webserv.event, 1, NULL, 0, NULL);
	}
	webserv.client->sent += send(webserv.event.ident, webserv.client->res.c_str() + webserv.client->sent, webserv.client->size - webserv.client->sent, 0);
	if ((webserv.client->size - webserv.client->sent) == 0)
	{
		std::cout << "\e[1;36m<-- Response has been sent successfully !!\e[0m\n\n";
		if (response.getClientRequest().getConnection() == "close")
		{
			delete webserv.client;
			close(webserv.event.ident);
			return ;
		}
		delete webserv.client;
		EV_SET(&webserv.event, webserv.client->socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
		kevent(webserv.kq, &webserv.event, 1, NULL, 0, NULL);
		webserv.client = new Client;
		bzero(webserv.client, sizeof(Client));
		webserv.client->socket = webserv.event.ident;
		EV_SET(&webserv.event, webserv.client->socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void *)(webserv.client));
		kevent(webserv.kq, &webserv.event, 1, NULL, 0, NULL);
	}
}

void	multipleServers(t_WebServ& webserv)
{
	std::map<std::string, int>::iterator it;

	webserv.kq = kqueue();
	it = webserv.ports.begin();
	while (it != webserv.ports.end())
	{
		bzero(&webserv.event, sizeof(webserv.event));
		webserv.port = it->second;
		SetupSocket(webserv);
		fcntl(webserv.sockfd, F_SETFL, O_NONBLOCK);
		EV_SET(&webserv.event, webserv.sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
		kevent(webserv.kq, &webserv.event, 1, NULL, 0, NULL);
		webserv.sockets.push_back(webserv.sockfd);
		it++;
	}
	multipleClient(webserv);
}

bool	isServer(t_WebServ& webserv)
{
	std::vector<int>::iterator	it;

	it = webserv.sockets.begin();
	while (it < webserv.sockets.end())
	{
		if (webserv.event.ident == (uintptr_t)(*it))
			return (true);
		it++;
	}
	return (false);
}

void	multipleClient(t_WebServ& webserv)
{
	while (1)
	{
		webserv.nev = kevent(webserv.kq, NULL, 0, &webserv.event, 1, NULL);
		if (webserv.nev < 0)
			throw std::runtime_error("kevent");
		if (webserv.event.flags & EV_EOF)
		{
			std::cout << "\e[0;31mClient terminated the session\e[0m\n";
			close(webserv.event.ident);
		}
		else if (isServer(webserv))
		{
			accept_connection(webserv);
			webserv.client = new Client;
			bzero(webserv.client, sizeof(Client));
			webserv.client->socket = webserv.acceptfd;
			EV_SET(&webserv.event, webserv.acceptfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void *)(webserv.client));
			kevent(webserv.kq, &webserv.event, 1, NULL, 0, NULL);
		}
		else if (webserv.event.filter == -1)
			recvRequest(webserv);
		else if (webserv.event.filter == -2)
			sendResponse(webserv);
	}
}
