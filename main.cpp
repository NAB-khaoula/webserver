/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ybouddou <ybouddou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:32:15 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/04 15:34:34 by ybouddou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int		main()
{
	try
	{
		WebServ	webserv;
		
		webserv.servers = begin_parser();
		// print_attr(webserv.servers);
		multipleServers(&webserv);
	}
	catch(const std::exception& err)
	{
		perror("Error");
		std::cout << err.what();
		std::cout << std::endl;
	}
	return (0);
}