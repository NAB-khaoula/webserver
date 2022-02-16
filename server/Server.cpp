/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:57:33 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/15 21:15:38 by ybouddou         ###   ########.fr       */
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

Request	recvRequest(t_WebServ *webserv)
{
	char	*buffer = new char[1024];
	std::string		req;
	size_t	ret;
	size_t	rd;
	size_t	last;
	// size_t	length;
	std::ofstream	outfile;

	// std::cout << "+++ Request +++\n";
	outfile.open("request.txt");
	bzero(buffer, 1024);
	ret = recv(webserv->event.ident, buffer, 1023, 0);
	rd = ret;
	req = buffer;
	bzero(buffer, 1024);
	std::cout << "+ret : " << ret << " | total : " << req.size() << "\n";
	// length = request.getContentLenght();
	while (1)
	{
		ret = recv(webserv->event.ident, buffer, 1023, 0);
		if ((int)ret < 0)
		{
			std::cout << "last read : " << ret << " | total : " << req.size() << "\n";
			// std::cout << strerror(errno) << " ==> " << errno << std::endl;
			// throw std::runtime_error("recv err.");
			break ;
		}
		if (ret == 0)
		{
			std::cout << "found zero\n";
			break ;
		}
		// std::cout << "ret : " << ret << " | total : " << req.size() << "\n";
		req += buffer;
		bzero(buffer, 1024);
		last = ret;
	}
	std::cout << "request size : " << req.size() << "\nlast read : " << last << "\nfirst read: " << rd << "\n\n";	
	Request		request(req);
	outfile << req << "++*--\n";
	outfile.close();
	delete[] buffer;
	return (request);
}

void	sendResponse(t_WebServ *webserv, Request request)
{
	webserv->client = (Client *)webserv->event.udata;
	Response resp(request, webserv->servers);
	std::string		res;
	std::ofstream	out_file;

	res = resp.returnResponse();
	out_file.open("response.txt");
	out_file << res;
	out_file.close();
	webserv->client->size = res.size();
	webserv->client->buffer = new char[webserv->client->size + 1];
	bzero(webserv->client->buffer, webserv->client->size + 1);
	std::strcpy(webserv->client->buffer, res.c_str());
	webserv->client->sent = send(webserv->event.ident, webserv->client->buffer, webserv->client->size, 0);
	if (webserv->client->sent == webserv->client->size)
	{
		delete[] webserv->client->buffer;
		delete[] webserv->client;
		close(webserv->event.ident);
	}
	webserv->client->rem = webserv->client->size - webserv->client->sent;
	// std::cout << "+response size : " << webserv->client->size << "\nsent : " << webserv->client->sent << "\nremaining: " << webserv->client->rem << "\n\n";
	// std::cout << "---------------------\n+++ Response +++\nresponse size : " << res.size() << std::endl;
	// delete[] buffer;
}

void	handle_connection(t_WebServ *webserv)
{
	Request	request;
	
	std::cout << "\e[0;36m-> Handling Connection!!\e[0m\n";
	request = recvRequest(webserv);
	sendResponse(webserv, request);
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
		// fcntl(sock.getSockfd(), F_SETFL, O_NONBLOCK);
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
			webserv->client = (Client *)malloc(sizeof(Client));
			bzero(webserv->client, sizeof(Client));
			fcntl(webserv->acceptfd, F_SETFL, O_NONBLOCK);
			EV_SET(&webserv->event, webserv->acceptfd, EVFILT_READ, EV_ADD, 0, 0, (void *)(webserv->client));
			kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
			// EV_SET(&webserv->event, webserv->acceptfd, EVFILT_WRITE, EV_ADD, 0, 0, (void *)(webserv->client));
			// kevent(webserv->kq, &webserv->event, 1, NULL, 0, NULL);
		}
		else if (webserv->event.filter & EVFILT_READ)
			handle_connection(webserv);
		// else if (webserv->event.filter & EVFILT_WRITE)
		// {
		// 	webserv->client = (Client *)webserv->event.udata;
		// 	webserv->client->sent = send(webserv->event.ident, webserv->client->buffer, webserv->client->size - webserv->client->sent, 0);
		// 	std::cout << "-response size : " << webserv->client->size << "\nsent : " << webserv->client->sent << "\nremaining: " << webserv->client->rem << "\n\n";
		// 	if (!webserv->client->sent)
		// 	{
		// 		delete[] webserv->client->buffer;
		// 		delete[] webserv->client;
		// 		close(webserv->event.ident);
		// 	}
		// }
	}
}
