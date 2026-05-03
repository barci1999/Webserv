/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 18:44:14 by pablalva          #+#    #+#             */
/*   Updated: 2026/05/03 19:16:21 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"
#include "Parseo_solo_toca_Pablo/Parser.hpp"
#include "Parseo_solo_toca_Pablo/Directive.hpp"
#include <iostream>

void log_server(const std::string host, const Directive& msg)
{
    std::cout <<BLUE <<"[SERVER] "<<RESET <<"host " << host <<" "<<msg.name << " ";
	for (std::vector<std::string>::const_iterator it = msg.args.begin(); it != msg.args.end() ;++it	 )
	{
		std::cout << *it << " ";
	}
	std::cout<<std::endl;
	
}

void log_request(const std::string& method,const std::string& path,const std::string& version,int status)
{
    std::cout <<YELLOW<<"[REQUEST VALUES]"<< RESET <<std::endl;
	std::cout << "method: " << method << std::endl;
	std::cout << "path: "<< path << std::endl;
	std::cout << "Version: "<< version << std::endl;
	if(status == 200 || status == 201 || status == 204)
		std::cout << LIGHT_MAGENTA <<"[RETURN] " <<RESET <<GREEN << status<<RESET<< std::endl;
	else
		std::cout << LIGHT_MAGENTA <<"[RETURN] " <<RESET <<RED << status<<RESET<< std::endl;
}