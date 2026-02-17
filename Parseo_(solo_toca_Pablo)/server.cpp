/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/17 20:09:00 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

server::server(const Block& to_check)
{
	this->_srvName = to_check.getName();
	this->_srvPorts = check_directives(std::string("listen"),to_check);
	this->_srvRoot = check_directives(std::string("root"),to_check);
	this->_srvIndex = check_directives(std::string("index"),to_check);
	this->_srvErrorPage = check_directives(std::string("error_page"),to_check);
	this->_srvLocations = to_check.getBlocks();
	check_locations(this->_srvLocations);
}
Directive server::check_directives(std::string to_search, const Block& to_check)
{
    const std::vector<Directive>& directives = to_check.getDirectives();
	std::vector<Directive>::const_iterator it = directives.begin();
    for (;it != directives.end(); ++it)
    {
        if (it->name == to_search && !it->args.empty())
            return *it;
		throw std::runtime_error("Empty directive.");
    }
    throw std::runtime_error("Directive not found");
	return *it;
}
bool server::check_locations(std::list<Block> to_check)
{
	std::list<Block>::iterator it = to_check.begin();
	for (; it != to_check.end(); it++)
	{
		if(!check_location_block(*it))
			throw std::runtime_error("invalid location block.");
	}
	return true;
}
bool server::check_location_block(Block to_check)
{
	
}


