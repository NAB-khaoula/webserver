/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelaman <mbelaman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/08 16:32:15 by ybouddou          #+#    #+#             */
/*   Updated: 2022/02/17 11:56:25 by mbelaman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int		main(int ac, char **av)
{
	try
	{
		if (ac == 2)
		{
			ws	webserv;
		
			begin_parser(webserv, av);
			// print_attr(webserv);
			multipleServers(&webserv);
		}
		else
			throw std::runtime_error("\033[1;31mSyntax Error: \033[0m\033[1;37mSpecify Output The Program and File Configuration like: './webserv [configuration file]'.\033[0m");
	}
	catch(const std::exception& err)
	{
		std::cout << err.what();
		std::cout << std::endl;
	}
	return (0);
}