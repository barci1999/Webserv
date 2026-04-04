/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/04 21:22:32 by pablalva         ###   ########.fr       */
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
	this->_srvAutoindex = check_directives(std::string("autoindex"),to_check);
	this->_srvLocations = to_check.getBlocks();
	check_locations(this->_srvLocations);
	this->_srvClientMaxBody = check_client_max_body(to_check);
}
server::server(){}
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
		}
	}
	throw std::runtime_error("Directive not found: " + to_search);
	return *it;
}
bool server::check_locations(std::list<Block> to_check)
{
	std::list<Block>::iterator it = to_check.begin();
	for (; it != to_check.end(); it++)
	{
		if(!check_location_block(*it))
			throw std::runtime_error("invalid location block.");
		else if (!cmp_name_directives(*it))
		{
			throw std::runtime_error("invalid directive block.");
		}
	}
	return true;
}
bool server::cmp_name_directives(const Block& to_check)
{
	std::string level[] = {"listen","root","index","error_page","client_max_body_size","autoindex","cgi_extension","upload_enable","upload_store","cgi_pass"};
	std::vector<Directive>::const_iterator it = to_check.getDirectives().begin();
	while (it != to_check.getDirectives().end())
	{
		bool result = false;
		for (size_t i = 0; i < 10; i++)
		{
			if (it->name == level[i])
			{
				result = true;
				break;
			}
		}
		if (!result)
		{
			return false;
		}
		++it;
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
			throw std::runtime_error("directives need at least one argument -> " + it->name);
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
size_t server::check_client_max_body(const Directive to_comprove)
{
	

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
static std::ostream& printVector(std::ostream& out, const std::vector<std::string>& v) {
    for (size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if (i != v.size() - 1) out << ", ";
    }
    return out;
}

// Función auxiliar para imprimir bloques recursivamente
static std::ostream& printBlock(std::ostream& out, const Block& b, int indent = 0) {
    std::string pad(indent, ' ');
    out << pad << "Block: " << b.getName() << std::endl;

    const std::vector<Directive>& dirs = b.getDirectives();
    for (size_t i = 0; i < dirs.size(); ++i) {
        out << pad << "  Directive: " << dirs[i].name << " -> ";
        printVector(out, dirs[i].args);
        out << std::endl;
    }

    const std::list<Block>& children = b.getBlocks();
    for (std::list<Block>::const_iterator it = children.begin(); it != children.end(); ++it) {
        printBlock(out, *it, indent + 4);
    }

    return out;
}

// Operador de salida para server
std::ostream& operator<<(std::ostream& out, const server& s) {
    out << "Server Name: " << s.get_srvName() << std::endl;

    out << "Ports: " << s.get_srvPorts().name << " -> ";
    printVector(out, s.get_srvPorts().args);
    out << std::endl;

    out << "Root: " << s.get_srvRoot().name << " -> ";
    printVector(out, s.get_srvRoot().args);
    out << std::endl;

    out << "Index: " << s.get_srvIndex().name << " -> ";
    printVector(out, s.get_srvIndex().args);
    out << std::endl;

    out << "ErrorPage: " << s.get_srvErrorPage().name << " -> ";
    printVector(out, s.get_srvErrorPage().args);
    out << std::endl;

    out << "Autoindex: " << s.get_srvAutoindex().name << " -> ";
    printVector(out, s.get_srvAutoindex().args);
    out << std::endl;

    out << "ClientMaxBodySize: " << s.get_srvClientMaxBody() << std::endl;

    out << "Locations:" << std::endl;
    const std::list<Block>& locs = s.get_srvLocations();
    for (std::list<Block>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
        printBlock(out, *it, 2);
    }

    return out;
}
Directive server::take_concret_direc(std::string to_search,std::vector<Directive> to_check)
{
	Directive result;
	for (std::vector<Directive>::iterator it = to_check.begin(); it != to_check.end(); ++it)
	{
		if (to_search == it->name)
		{
			result.name = it->name;
			result.args = it->args;
			return result;
		}
	}
	std::runtime_error("Directive not found -> "+ to_search);
	return result;
}
int server::insert_directives(const std::vector<Directive>& to_insert)
{
	this->_srvPorts = take_concret_direc("listen",to_insert);
	this->_srvRoot = take_concret_direc("root",to_insert);
	this->_srvIndex = take_concret_direc("index",to_insert);
	this->_srvErrorPage = take_concret_direc("error_page",to_insert);
	this->_srvAutoindex = take_concret_direc("autoindex",to_insert);
	this->_srvClientMaxBody = 
	
}