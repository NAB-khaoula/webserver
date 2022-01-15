#include "../WebServ.hpp"

Sockets::Sockets()
{
}

Sockets::~Sockets()
{
}

int		Sockets::SetupSocket(int port, std::string host)
{
	int		flag = 1;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servadd, sizeof(servadd));
	servadd.sin_family = AF_INET;
	if (host.empty())
		host = "127.0.0.1";
	servadd.sin_addr.s_addr = inet_addr(host.c_str());
	servadd.sin_port = htons(port);
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
		throw "setsockopt failed";
	if (bind(sockfd, (const struct sockaddr *) &servadd, sizeof(servadd)) < 0)
		throw "bind failed";
	if (listen(sockfd, 0) < 0)
		throw "listen failed";
	// getsockname(sockfd, (struct sockaddr *) &servadd, (socklen_t *)sizeof(servadd));
	// std::cout << "Local IP address is: " << inet_ntoa(servadd.sin_addr) << "\n";
	// std::cout << "Local port is: " << ntohs(servadd.sin_port) << "\n";
	return (sockfd);
}

int		Sockets::getSockfd() const
{
	return (sockfd);
}
