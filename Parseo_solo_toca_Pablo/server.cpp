/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by pablalva          #+#    #+#             */
/*   Updated: 2026/05/03 18:19:18 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

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
void server::validate_directives_names(const Block& to_check)
{
    std::string level[] = {
        "server_name","listen","root","index","error_page",
        "client_max_body_size","autoindex","cgi_extension",
        "upload_enable","upload_store","cgi_pass","allowed_methods"
    };
    for (std::vector<Directive>::const_iterator it = to_check.getDirectives().begin(); 
        it != to_check.getDirectives().end(); ++it)
    {
        bool valid = false;
        for (size_t i = 0; i < 12; ++i)
        {
            if (it->name == level[i])
            {
                valid = true;
                break;
            }
        }
        if (!valid)
        {
            throw std::runtime_error("Invalid directive name: " + it->name);
        }
    }
	if (check_root(take_concret_direc("root",to_check.getDirectives())).name.empty())
	{
		throw std::runtime_error("location block without root directive ->" + to_check.getName());
	}
	
}
Directive server::check_autoindex(const Directive& to_check)
{
    // no existe → vacío
    if (to_check.name.empty())
        return Directive();

    if (to_check.name != "autoindex")
        return Directive();

    if (to_check.args.size() != 1)
        throw std::runtime_error("autoindex requires exactly one argument");

    if (to_check.args[0] != "on" && to_check.args[0] != "off")
        throw std::runtime_error("autoindex must be 'on' or 'off'");
    return to_check;
}
bool server::check_location_block(const Block& loc)
{
    const std::vector<Directive>& directives = loc.getDirectives();
	const char* dirs[] = {
		"root", "index", "autoindex", "allowed_methods",
		"cgi_extension", "upload_enable", "upload_store", "cgi_pass"
	};
	std::set<std::string> valid_directives;
	for (size_t i = 0; i < 8; ++i) {
		valid_directives.insert(dirs[i]);
	}
	std::set<std::string> seen_directives; 
    for (std::vector<Directive>::const_iterator it = directives.begin(); it != directives.end(); ++it)
    {
        const Directive& d = *it;
        if (d.name.empty())
            throw std::runtime_error("Directive in location cannot have empty name");
        if (valid_directives.find(d.name) == valid_directives.end())
            throw std::runtime_error("Invalid directive in location: " + d.name);
        if (seen_directives.find(d.name) != seen_directives.end())
            throw std::runtime_error("Duplicate directive in location: " + d.name);
        seen_directives.insert(d.name);
        if (d.args.empty())
            throw std::runtime_error("Directive " + d.name + " requires at least one argument");
        if (d.name == "root")
        {
            if (d.args.empty())
                throw std::runtime_error("root args cant be empty");
        }
        else if (d.name == "autoindex")
        {
            if (d.args[0] != "on" && d.args[0] != "off")
                throw std::runtime_error("autoindex must be 'on' or 'off'");
        }
        else if (d.name == "allowed_methods")
        {
            for (size_t i = 0; i < d.args.size(); ++i)
            {
                if (d.args[i] != "GET" && d.args[i] != "POST" && d.args[i] != "DELETE")
                    throw std::runtime_error("Invalid HTTP method in allowed_methods: " + d.args[i]);
            }
        }
        else if (d.name == "cgi_pass")
        {
            if (d.args.empty() || d.args.size() > 2)
	            throw std::runtime_error("cgi_pass must have 1 or 2 arguments");
        }
        else if (d.name == "upload_enable")
        {
            if (d.args[0] != "on" && d.args[0] != "off")
                throw std::runtime_error("upload_enable must be 'on' or 'off'");
        }
        else if (d.name == "upload_store")
        {
            if (d.args[0].empty() || d.args[0][0] != '/')
                throw std::runtime_error("upload_store must be a valid absolute path starting with '/'");
        }
        else if (d.name == "cgi_extension")
        {
            for (size_t i = 0; i < d.args.size(); ++i)
            {
                if (d.args[i].empty() || d.args[i][0] != '.')
                    throw std::runtime_error("cgi_extension arguments must start with a dot '.' -> " + d.args[i]);
            }
        }
    }
    return true;
}
size_t server::check_client_max_body(const Directive to_comprove)
{
	
	if (to_comprove.args.size() != 1)
		throw std::runtime_error("client_max_body_size requires exactly one argument");
	std::string value = to_comprove.args[0];
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
    return out;
}

// Operador de salida para server
std::ostream& operator<<(std::ostream& out, const server& s) {
    out << "Server Name: " << s.get_srvName() << std::endl;

    out << "Ports: " << s.get_srvPorts().name << " -> ";
    printVector(out, s.get_srvPorts().args);
    out << std::endl;

	out << "Host: " << s.get_srvHost().name << " -> ";
    printVector(out, s.get_srvHost().args);
    out << std::endl;

    out << "Root: " << s.get_srvRoot().name << " -> ";
    printVector(out, s.get_srvRoot().args);
    out << std::endl;

    out << "Index: " << s.get_srvIndex().name << " -> ";
    printVector(out, s.get_srvIndex().args);
    out << std::endl;

    out << "ErrorPages:" << std::endl;
    const std::vector<Directive>& error_pages = s.get_srvErrorPage();
    for (std::vector<Directive>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
        out << "  " << it->name << " -> ";
        printVector(out, it->args);
        out << std::endl;
    }

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
Directive server::take_concret_direc(const std::string& to_search, const std::vector<Directive>& to_check)
{
    for (std::vector<Directive>::const_iterator it = to_check.begin(); it != to_check.end(); ++it)
    {
        if (to_search == it->name)
            return *it;
    }
    return Directive();
}
Directive server::check_error_page(const Directive& to_check)
{
    if (to_check.name != "error_page" || to_check.args.size() < 2)
        return Directive();

    Directive clean = to_check;

    for (size_t i = 0; i + 1 < clean.args.size(); ++i)
    {
        int code = std::atoi(clean.args[i].c_str());
        if (code < 400 || code > 599)
            return Directive();
    }

    std::string& path = clean.args.back();

    if (path.empty())
        return Directive();

    if (!path.empty() && path[0] == '/')
	{
        path.erase(0, 1);
	}
    return clean;
}
Directive server::check_index(const Directive& to_check)
{
    if (to_check.name.empty() || to_check.name != "index")
        return Directive();

    if (to_check.args.empty())
	{
        return Directive();
	}	
	Directive clean = to_check;

    std::string& idx = clean.args[0];

    if (!idx.empty() && idx[0] == '/')
	{
        idx.erase(0,1);
	}
    return clean;
}
Directive server::check_root(const Directive& to_check)
{
    if (to_check.name != "root" || to_check.args.empty())
        return Directive();

    Directive clean = to_check;
    std::string& path = clean.args[0];

    if (!path.empty() && path[0] == '/')
        path.erase(0, 1);

    return clean;
}
Directive server::check_listen(const Directive& to_check)
{
    if (to_check.name.empty() || to_check.name != "listen")
        return Directive();
    if (to_check.args.empty())
        return Directive();
    for (size_t i = 0; i < to_check.args.size(); ++i)
    {
        int port = std::atoi(to_check.args[i].c_str());
        if (port <= 0 || port > 65535)
            return Directive();
    }
    return to_check;
}
Directive server::check_host(const Directive& to_check)
{
	
	if (to_check.name.empty() || to_check.name != "host")
	{
        return Directive();
	}
	if (to_check.args.empty() || to_check.args.size() != 1)
	{
        return Directive();
	}
	Directive clean = to_check;
	std::string& ip = clean.args[0];
	std::vector<std::string> tokens;
	std::stringstream input(ip);
	std::string buffer;
	while (std::getline(input,buffer,'.'))
		tokens.push_back(buffer);
	if (tokens.empty() || tokens.size() != 4)
	{
		return Directive();
	}
	for (size_t i= 0; i < tokens.size(); ++i)
	{
		if(!is_valid_number(tokens[i]))
		{
			return Directive();
		}
		int value = std::atoi(tokens[i].c_str());
		if (value < 0 || value > 255)
		{
			return Directive();
		}
		if (tokens[i].size() > 1 && tokens[i][0] == '0')
        {
			return Directive();
		}
	}
	int first = std::atoi(tokens[0].c_str());
	if (first >= 224)
	{
		return Directive();
	}
	if (ip == "255.255.255.255")
	{
		return Directive();
	}
	return clean;
}
int server::insert_directives(const std::vector<Directive>& to_insert)
{
    std::set<std::string> seen;
    for (std::vector<Directive>::const_iterator it = to_insert.begin(); it != to_insert.end(); ++it)
    {
        if (it->name == "error_page") 
        {
			Directive checked = check_error_page(*it);
			if (checked.name.empty())
				throw std::runtime_error("Invalid error_page directive");
			this->_srvErrorPage.push_back(checked);
			continue;
        }

        if (seen.find(it->name) != seen.end())
            throw std::runtime_error("Duplicate directive not allowed: " + it->name);

        seen.insert(it->name);

        if (it->name == "listen")          this->_srvPorts = check_listen(*it);
        else if (it->name == "root")      this->_srvRoot = check_root(*it);
        else if (it->name == "index")     this->_srvIndex = check_index(*it);
        else if (it->name == "autoindex") this->_srvAutoindex = check_autoindex(*it);
		else if (it->name == "host") this->_srvHost = check_host(*it);
        else if (it->name == "client_max_body_size")
            this->_srvClientMaxBody = check_client_max_body(*it);
    }
    if (this->_srvPorts.name.empty())
        throw std::runtime_error("Missing mandatory directive: listen");
    if (this->_srvRoot.name.empty())
	{
        throw std::runtime_error("Missing mandatory directive: root");
	}
	if (this->_srvHost.name.empty())
	{
		throw std::runtime_error("Invalid ip");
	}
    if (this->_srvAutoindex.name.empty())
    {
        this->_srvAutoindex.name = "autoindex";
        this->_srvAutoindex.args.push_back("off");
    }
    if (this->_srvClientMaxBody == 0)
    {
        this->_srvClientMaxBody = 1048576;
    }
    return 0;
}
void server::normalize_location(Block& loc)
{
    std::vector<Directive>& directives = loc.getDirectivesMutable();

    for (size_t i = 0; i < directives.size(); ++i)
    {
        for (size_t j = 0; j < directives[i].args.size(); ++j)
        {
            if (!directives[i].args[j].empty() &&
                directives[i].args[j][0] == '/')
            {
                directives[i].args[j].erase(0, 1);
            }
        }
    }
}
int server::insert_locations(const std::list<Block>& to_insert)
{
    std::set<std::string> location_names;

    for (std::list<Block>::const_iterator it = to_insert.begin(); it != to_insert.end(); ++it)
    {
		Block clean = *it;
		normalize_location(clean);
        check_location_block(clean);
		if (take_concret_direc("root", clean.getDirectives()).name.empty())
		{
			throw std::runtime_error("No root directive detected in block -> " + it->getName());
		}
		
        const std::string& loc_name = clean.getName();
        if (location_names.find(loc_name) != location_names.end())
            throw std::runtime_error("Duplicate location block -> " + loc_name);
        location_names.insert(loc_name);
        this->_srvLocations.push_back(clean);
    }

    return 0;
}
