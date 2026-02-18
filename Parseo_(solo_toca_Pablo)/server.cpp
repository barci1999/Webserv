/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/18 15:50:35 by pablalva         ###   ########.fr       */
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
	this->_srvAutoIndex = check_autoindex(to_check);
    this->_srvClientMaxBody = check_client_max_body(to_check);
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
bool server::check_autoindex(const Block& to_check)
{
    const std::vector<Directive>& directives = to_check.getDirectives();
    for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); ++it)
    {
        if (it->name == "autoindex")
        {
            if (it->args.size() != 1)
                throw std::runtime_error("autoindex requires exactly one argument");
            if (it->args[0] == "on")
                return true;
            if (it->args[0] == "off")
                return false;
            throw std::runtime_error("autoindex must be 'on' or 'off'");
        }
    }
    return false;
}
bool server::check_location_block(Block to_check)
{
	
}
size_t server::check_client_max_body(const Block& to_check)
{
    Directive temp = check_directives("client_max_body_size", to_check);

    if (temp.args.size() != 1)
        throw std::runtime_error("client_max_body_size requires exactly one argument");
    std::string value = temp.args[0];
    if (value.empty())
        throw std::runtime_error("client_max_body_size empty value");
    size_t multiplier = 1;
    char last = value[value.size() - 1];

    if (last == 'k' || last == 'K')
    {
        multiplier = 1024;
        value.erase(value.size() - 1);
    }
    else if (last == 'm' || last == 'M')
    {
        multiplier = 1024 * 1024;
        value.erase(value.size() - 1);
    }
    else if (last == 'g' || last == 'G')
    {
        multiplier = 1024 * 1024 * 1024;
        value.erase(value.size() - 1);
    }
    for (size_t i = 0; i < value.size(); ++i)
    {
        if (!std::isdigit(value[i]))
            throw std::runtime_error("invalid number in client_max_body_size");
    }
    std::istringstream iss(value);
    size_t number;
    iss >> number;
    if (iss.fail())
        throw std::runtime_error("conversion error in client_max_body_size");
    return number * multiplier;
}



