/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockets.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/13 08:44:06 by ybouddou          #+#    #+#             */
/*   Updated: 2022/01/13 11:50:10 by ybouddou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETS_HPP
# define SOCKETS_HPP

#include "../parsing/Server.hpp"
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/event.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Sockets
{
	private:
		struct sockaddr_in	servadd;
		int					sockfd;
	public:
		Sockets();	
		~Sockets();
		int	SetupSocket(int port, std::string host);
		int	getSockfd() const;
};

#endif