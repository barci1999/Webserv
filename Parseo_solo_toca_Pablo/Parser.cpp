/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 01:10:56 by pablo             #+#    #+#             */
/*   Updated: 2026/04/05 16:15:01 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include"Parser.hpp"
#include<sstream>

Parser::Parser(){}
Parser::Parser(const Parser& other)
{
	(void)other;
}
Parser& Parser::operator=(const Parser& other)
{
	(void)other;
	return *this;
}
Parser::~Parser(){}

std::string Parser::trim(std::string input)
{
   size_t start = 0;
   while (start < input.length() && std::isspace((unsigned char)input[start]))
		start++;
   size_t end = input.length();
   while (end > start && std::isspace((unsigned char)input[end - 1]))
		end--;
   return(input.substr(start,end - start));  
}

tokens Parser::tokenize(const std::string& line)
{
	std::string to_check = trim(line);
	std::vector<std::string> temp = str_to_vector(to_check);
	if (temp.empty())
		return EMPTY;
	if (temp[0] == "#")
		return COMMENT;
	if (temp[0] == "server" && temp.size() > 1 && temp.back() == "{")
	{
		return SERVER_BLOCK_START;
	}
	if (temp.back() == "{")
		return BLOCK_START;
	if (temp.back() == "}")
		return BLOCK_END;
	if (temp.back()== ";")
		return DIRECTIVE;
	return ERROR;
}
Directive Parser::parseDirective(const std::string& line)
{
    Directive result;
    std::string level[] = {
        "server_name","listen","root","index","error_page",
        "client_max_body_size","autoindex","cgi_extension",
        "upload_enable","upload_store","cgi_pass","allowed_methods"
    };
    std::string to_check = trim(line);
    if (to_check.empty() || to_check[to_check.length() - 1] != ';')
        throw std::runtime_error("missing ';' in directive: " + line);
    to_check.erase(to_check.length() - 1);
    std::istringstream iss(to_check);
    std::string token;
    if (iss >> token)
    {
        bool match = false;
        size_t size = sizeof(level) / sizeof(level[0]);
        for (size_t i = 0; i < size; i++)
        {
            if (token == level[i])
            {
                match = true;
                break;
            }
        }
        if (!match)
            throw std::runtime_error("invalid directive detected: " + token);
        result.name = token;
    }
    if (result.name.empty())
        throw std::runtime_error("invalid directive format: " + line);
    while (iss >> token)
        result.args.push_back(token);
    if (result.args.empty())
        throw std::runtime_error("directive missing arguments: " + result.name);
    return result;
}
Block Parser::parseBlock(std::ifstream& file, const std::string& line)
{
	Block location;
	std::vector<std::string> tmp = str_to_vector(line);
	if (tmp.empty() || tmp.size() !=  3 || tmp.back() != "{" || tmp[0] != "location")
	{
		throw std::runtime_error("Invalid location block header -> " + line);
	}
	location.setName(tmp[1]);
	std::string file_line;
	while (std::getline(file,file_line))
	{
		tokens token = tokenize(file_line);
		if (token == EMPTY || token == COMMENT)
		{
			continue;
		}
		else if (token == DIRECTIVE)
		{
			location.addDirective(parseDirective(file_line));
		}
		else if (token == BLOCK_START || token == SERVER_BLOCK_START)
		{
			throw std::runtime_error("Nested blocks are not allowed -> " +trim(file_line));
		}
		else if (token == BLOCK_END)	
		{
			return location;
		}
		else
		{
			throw std::runtime_error("Invalid line insida loction -> " + trim(file_line));
		}
	}
	throw std::runtime_error("Missing closing brace in location block -> " + trim(line));
	return location;
}
std::vector<server> Parser::parseFile(const std::string& file_name)
{
	std::vector<server> result;
	std::ifstream file(file_name.c_str());
	if(!file.is_open())
		throw std::runtime_error("Error open file");
	std::string line;
	while (std::getline(file,line))
	{
		tokens type = tokenize(line);
		if (type == COMMENT || type == EMPTY)
		{
			continue;
		}
		else if(type == SERVER_BLOCK_START)
		{
			server to_insert = parseServerBlock(file,line);
			result.push_back(to_insert);
		}
		else
		{
			throw std::runtime_error("Unexpected line outside server block: " + line);
		}
		
	}
	//server.insert_directives(directives);
	//server.insert_blocks(locations);
	return result;
}
server Parser::parseServerBlock(std::ifstream& file,std::string line)
{
	server Server;
	std::vector<Directive> directives;
	std::list<Block> locations;
	std::vector<std::string> server_name = str_to_vector(line);
	if (server_name.empty() || (server_name.size() != 3 && server_name.size() != 2) || server_name.back() != "{")
	{
		throw std::runtime_error("invalid line detected ->" + line);
	}
	else
	{
		if (server_name.size() == 2)
		{
			Server.set_srvName("default");
		}
		else
		{
			Server.set_srvName(server_name[1]);
		}
	}
	while (std::getline(file,line))
	{
		tokens token = tokenize(line);
		if (token == EMPTY || token == COMMENT)
		{
			continue;
		}
		else if (token == DIRECTIVE)
		{
			Directive insert_directive = parseDirective(line);
			directives.push_back(insert_directive);
		}
		else if (token == BLOCK_START)
		{
			Block insert_block = parseBlock(file,line);
			locations.push_back(insert_block);
		}
		else if (token == BLOCK_END)
		{
			Server.insert_directives(directives);
			Server.insert_locations(locations);
			return Server;
		}
		else
		{
			throw std::runtime_error("invalid line -> " + line);
		}
	}
	throw std::runtime_error("Missing closing brace in server block");
	return Server;
}

std::vector<std::string> Parser::str_to_vector(const std::string& to_change)
{
	std::string to_check = trim(to_change);
	std::vector<std::string> result;

	std::istringstream iss(to_check);
	std::string token;

	while (iss >> token)
		result.push_back(token);

	return result;
}
std::string Parser::vector_to_str(const std::vector<std::string> to_change)
{
	std::string result = "";
	for (std::vector<std::string>::const_iterator it = to_change.begin(); it != to_change.end(); it++)
	{
		if (it == to_change.begin())
			result += *it;
		else
			result += " "  + *it;
	}
	return result;
}