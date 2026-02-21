/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/21 21:27:52 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

server::server(const Block& to_check)
{
	this->_srvName = to_check.getName();
    std::cout<<this->_srvName<<std::endl;
	this->_srvPorts = check_directives(std::string("listen"),to_check);
	this->_srvRoot = check_directives(std::string("root"),to_check);
	this->_srvIndex = check_directives(std::string("index"),to_check);
	this->_srvErrorPage = check_directives(std::string("error_page"),to_check);
	this->_srvLocations = to_check.getBlocks();
	check_locations(this->_srvLocations);
	this->_srvAutoIndex = check_autoindex(to_check);
    this->_srvClientMaxBody = check_client_max_body(to_check);
    for (size_t i = 0; i < this->_srvPorts.args.size(); i++)
    {
        this->_srvListeners.push_back(listener(this->_srvPorts.args[i]));
    }
    
}
Directive server::check_directives(std::string to_search, const Block& to_check)
{
    const std::vector<Directive>& directives = to_check.getDirectives();
	std::vector<Directive>::const_iterator it = directives.begin();
    for (;it != directives.end(); ++it)
    {
        if (it->name == to_search && !it->args.empty())
        {
            return *it;
        }
        else if (it->name == to_search && !it->args.empty())
        {
            throw std::runtime_error("Empty directive.");
            /* code */
        }
        
    }
    std::cout << to_search << to_check.getName() <<std::endl;
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
bool server::check_location_block(const Block& to_check)
{
    const std::vector<Directive>& directives = to_check.getDirectives();
    check_directives("root", to_check);
    for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); ++it)
    {
        if (it->name.empty())
            throw std::runtime_error("directives need a name");
        if (it->args.empty())
            throw std::runtime_error("directives need at least one argument");
    }
    const std::list<Block>& children = to_check.getBlocks();
    for (std::list<Block>::const_iterator bit = children.begin(); bit != children.end(); ++bit)
    {
        std::vector<std::string> temp = Parser::str_to_vector(bit->getName());
        if (temp.empty())
            throw std::runtime_error("invalid limit_except block name");
        if (temp[0] != "limit_except")
            throw std::runtime_error("invalid block inside location");
        if (temp.size() < 2)
            throw std::runtime_error("limit_except requires at least one method");
        for (size_t i = 1; i < temp.size(); ++i)
        {
            if (temp[i] != "GET" && temp[i] != "POST" && temp[i] != "DELETE")
                throw std::runtime_error("invalid HTTP method in limit_except");
        }
        const std::vector<Directive>& child_directives = bit->getDirectives();
        if (child_directives.size() != 1)
            throw std::runtime_error("limit_except must contain exactly one directive");
        const Directive& d = child_directives[0];
        if (d.name != "deny")
            throw std::runtime_error("only 'deny' allowed in limit_except");
        if (d.args.size() != 1 || d.args[0] != "all")
            throw std::runtime_error("deny directive must be: deny all");
    }
    return true;
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
server::~server(){}



