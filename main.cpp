/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelaman <mbelaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:32:15 by ybouddou          #+#    #+#             */
/*   Updated: 2022/01/20 17:52:17 by mbelaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int		main()
{
	try
	{
		WebServ	webserv;
		
		webserv.servers = begin_parser();
		print_attr(webserv.servers);
		// multipleServers(&webserv);
	}
	catch(const char* err)
	{
		std::cout << err << std::endl;
	}
	return (0);
}