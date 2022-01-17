/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelaman <mbelaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:32:15 by ybouddou          #+#    #+#             */
/*   Updated: 2022/01/16 18:25:39 by mbelaman         ###   ########.fr       */
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
	catch(std::string err)
	{
		std::cout << err << std::endl;
	}
	return (0);
}