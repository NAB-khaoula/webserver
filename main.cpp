/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelaman <mbelaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:32:15 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/14 16:41:15 by mbelaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int		main()
{
	try
	{
		ws	webserv;
		
		begin_parser(webserv);
		// print_attr(webserv);
		multipleServers(&webserv);
	}
	catch(const std::exception& err)
	{
		std::cout << err.what();
		std::cout << std::endl;
	}
	return (0);
}