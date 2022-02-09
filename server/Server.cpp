/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/09 18:10:05 by ybouddou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../WebServ.hpp"
#include <sys/ioctl.h>

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

void	Request::parseBody(std::string req)
{
	std::ofstream	in_file;
	size_t			pos;
	
	in_file.open("body.txt");
	pos = req.find("\r\n\r\n");
	if (pos != std::string::npos)
		req.erase(0, pos + 4);
	in_file << req << "\n";
	in_file.close();
}

void	recvRequest(Request& request, char *buffer, struct kevent event)
{
	std::string		req;
	size_t	ret;
	size_t	rd;
	size_t	length;
	std::ofstream	in_file;

	// std::cout << "+++ Request +++\n";
	in_file.open("request.txt");
	buffer = new char[event.data + 1];
	memset(buffer, 0, event.data + 1);
	ret = recv(event.ident, buffer, event.data, 0);
	buffer[ret] = '\0';
	req = buffer;
	request.parseRequest(buffer);
	length = request.getContentLenght();
	while (ret < length)
	{
		memset(buffer, 0, event.data + 1);
		rd = recv(event.ident, buffer, event.data, 0);
		buffer[rd] = '\0';
		req += buffer;
		ret += rd;
	}
	// std::cout << "request size : " << req.size() << "\ntotal read : " << ret << "\nContent lenght : " << length << "\nevent.data : " << event.data << "\n\n";
	request.parseBody(req);
	in_file << req << "\n";
	in_file.close();
	delete[] buffer;
}

void	sendResponse(Request& request, char *buffer, struct kevent event, std::vector<Server>& servers)
{
	Response resp(request, servers);
	std::string		res;
	std::ofstream	out_file;
	
	out_file.open("response.txt");
	res = resp.buildResponse();
	out_file << res << "\n";
	out_file.close();
	buffer = new char[res.size() + 1];
	std::strcpy(buffer, res.c_str());
	send(event.ident, buffer, strlen(buffer), 0);
	// std::cout << "---------------------\n+++ Response +++\nresponse size : " << res.size() << std::endl;
	delete[] buffer;
}

void	handle_connection(t_WebServ *webserv, struct kevent event)
{
	Request	request;
	char	*buffer;
	
	std::cout << "\e[0;36m-> Handling Connection!!\e[0m\n";
	recvRequest(request, buffer, event);
	sendResponse(request, buffer, event, webserv->servers);
	close(event.ident);
}

void	multipleServers(t_WebServ *webserv)
{
	Sockets		sock;
	struct kevent	change;
	std::map<std::string, int>::iterator it;

	webserv->kq = kqueue();
	it = webserv->ports.begin();
	while (it != webserv->ports.end())
	{
		memset(&webserv->event, 0, sizeof(webserv->event));
		sock.SetupSocket(it->second);
		// std::cout << it->second << " | 127.0.0.1" << std::endl;
		EV_SET(&webserv->event, sock.getSockfd(), EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, NULL);
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
			EV_SET(&webserv->event, webserv->acceptfd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
			kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		}
		else if (webserv->event.filter & EVFILT_READ)
			handle_connection(webserv, webserv->event);
	}
}
