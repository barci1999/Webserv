/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 01:10:56 by pablo             #+#    #+#             */
/*   Updated: 2026/02/21 21:21:48 by pablo            ###   ########.fr       */
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
	if (to_check.empty())
		return EMPTY;
	if (to_check[0] == '#')
		return COMMENT;
	if (to_check[to_check.length() - 1] == '{')
		return BLOCK_START;
	if (to_check[to_check.length() - 1] == '}')
		return BLOCK_END;
	if (to_check[to_check.length() - 1] == ';')
		return DIRECTIVE;
	return ERROR;
}
Directive Parser::parseDirective(const std::string& line)
{
	Directive result;
	std::string to_check = trim(line);
	if (!to_check.empty() && to_check[to_check.length() - 1] == ';')
	{
		to_check.erase(to_check.length()-1);
	}
	std::istringstream iss(to_check);
	std::string token;
	if (iss >> token)
    	result.name = token;
	while (iss >> token)
	{
		result.args.push_back(token);
	}
	return result;
	
}
Block Parser::parseBlock(std::ifstream& file, const std::string& block_name)
{
    std::string cleaned_name = trim(block_name);
    if (!cleaned_name.empty() && cleaned_name[cleaned_name.length() - 1] == '{')
        cleaned_name.erase(cleaned_name.length() - 1);
    cleaned_name = trim(cleaned_name);

    Block result(cleaned_name);
    std::string file_line;

    while (std::getline(file, file_line))
    {
        std::string temp = trim(file_line);
        if (temp.empty() || temp[0] == '#') 
			continue;

        tokens type = tokenize(temp);

        if (type == BLOCK_END)
            return result;
        else if (type == DIRECTIVE)
            result.addDirective(parseDirective(temp));
        else if (type == BLOCK_START)
            result.addChild(parseBlock(file, temp));
        else if (type == ERROR)
		{
			std::cout << file_line<< std::endl;
            throw std::runtime_error("Syntax error detected");
		}
    }

    throw std::runtime_error("Missing closing brace");
}
Block Parser::parseFile(const std::string& file_name)
{
	Block result("root");
	std::ifstream file(file_name.c_str());
	if(!file.is_open())
		throw std::runtime_error("Error open file");
	std::string line;
	while (std::getline(file,line))
	{
		switch (tokenize(line))
		{
		case EMPTY:
			break;
		case DIRECTIVE:
			result.addDirective(parseDirective(line));
			break;
		case BLOCK_START:
			result.addChild(parseBlock(file,line));
			break;
		case BLOCK_END:
			break;
		case COMMENT:
			break;
		case ERROR:
			throw std::runtime_error("token Error detected");
		}
	}
	return result;
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