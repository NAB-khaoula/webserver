/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/17 22:32:19 by ybouddou         ###   ########.fr       */
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
	// getsockname(sockfd, (struct sockaddr *) &servadd, (socklen_t *)sizeof(servadd));
	// std::cout << "Local IP address is: " << inet_ntoa(servadd.sin_addr) << "\n"; 
	// std::cout << "Local port is: " << ntohs(servadd.sin_port) << "\n";
	return (sockfd);
}

int		accept_connection(int sockfd)
{
	int					acceptfd;
	// struct sockaddr_in	clientadd;
	// socklen_t			add_len;
	// char				client_address[6000] = {0};
	// uint16_t			client_port;

	acceptfd = accept(sockfd, NULL, NULL);
	std::cout << "\e[0;34m-> Client accepted !!\e[0m\n";
	// acceptfd = accept(sockfd, (struct sockaddr *) &clientadd, &add_len);
	// inet_ntop(AF_INET, &clientadd, client_address, 6000);
	// client_port = ntohs(clientadd.sin_port);
	// std::cout << "Accepted fd : " << acceptfd << " Connection from " << client_address << ", port " << client_port << "\n\n";
	return (acceptfd);
}

void	recvRequest(t_WebServ *webserv)
{
	size_t	pos;
	
	webserv->client = (Client *)webserv->event.udata;
	bzero(webserv->client->receive, 1024);
	webserv->client->ret = recv(webserv->event.ident, webserv->client->receive, 1023, 0);
	webserv->client->req += webserv->client->receive;
	if (webserv->client->header.empty())
	{
		if((pos = webserv->client->req.find("\r\n\r\n")) != std::string::npos)
		{
			std::ofstream	headerfile("header.txt");
			webserv->client->header = webserv->client->req.substr(0, pos + 4);
			webserv->client->req.erase(0, pos + 4);
			webserv->client->lenght = 0;
			headerfile << webserv->client->header;
			if((pos = webserv->client->header.find("Content-Length: ")) != std::string::npos)
			{
				std::string	tmp = webserv->client->header;
				tmp.erase(0, pos + 16);
				webserv->client->lenght = stoi(tmp.substr(0, tmp.find("\r\n")));
				std::cout << "| lenght: " << webserv->client->lenght << " |\n";
			}
		}
	}
	if (!webserv->client->header.empty() && webserv->client->req.size() == webserv->client->lenght)
	{
		std::ofstream	outfile("request.txt");
		webserv->client->req = webserv->client->header + webserv->client->req;
		outfile << webserv->client->req;
		webserv->client->request = Request(webserv->client->req);
		webserv->client->req = "";
		EV_SET(&webserv->event, webserv->client->socket, EVFILT_READ, EV_DELETE, 0, 0, (void *)(webserv->client));
		kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		sendResponse(webserv);
		EV_SET(&webserv->event, webserv->client->socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, (void *)(webserv->client));
		kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		outfile.close();
	}
	std::cout << "| " << webserv->client->req.size() << " | " << webserv->client->lenght << " |\n";
}

void	sendResponse(t_WebServ *webserv)
{
	webserv->client = (Client *)webserv->event.udata;

	if (webserv->client->res.empty())
	{
		std::ofstream	out_file("response.txt");
		Response response(webserv->client->request, webserv->servers);
		webserv->client->res = response.returnResponse();
		out_file << webserv->client->res;
		out_file.close();
		webserv->client->size = webserv->client->res.size();
		webserv->client->send = new char[webserv->client->size + 1];
		bzero(webserv->client->send, webserv->client->size + 1);
		std::strcpy(webserv->client->send, webserv->client->res.c_str());
		webserv->client->sent = send(webserv->event.ident, webserv->client->send, webserv->client->size, 0);
		webserv->client->rem = webserv->client->size - webserv->client->sent;
	}
	else
	{
		webserv->client->sent += send(webserv->event.ident, webserv->client->send + webserv->client->sent, webserv->client->rem, 0);
		webserv->client->rem = webserv->client->size - webserv->client->sent;
	}
	std::cout << "| sent: " << webserv->client->sent << " | " << webserv->client->rem << " |\n";
	if (!webserv->client->rem)
	{
		delete[] webserv->client->send;
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
		std::cout << "\e[1;32mWaiting for connection ...\e[0m\n";
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
		{
			std::cout << "\e[0;36m-> Handling Read!!\e[0m\n";
			recvRequest(webserv);
		}
		else if (webserv->event.filter == -2)
		{
			std::cout << "\e[0;36m-> Handling Write!!\e[0m\n";
			sendResponse(webserv);
		}
	}
}
