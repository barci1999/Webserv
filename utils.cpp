/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:29:06 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/07 17:27:30 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

Directive search_directive(std::string to_cheack,const Block block)
{
	Directive result;
	result.name = "";
	const std::vector<Directive>& directive = block.getDirectives();
	std::vector<Directive>::const_iterator it = directive.begin();
	for (; it != directive.end(); it++)
	{
		if (to_cheack == it->name)
		{
			result.name = it->name;
			result.args = it->args;
			return result;
		}
	}
	return result;
}
std::string toString(unsigned int value) 
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
bool is_directory(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        return false;
    return S_ISDIR(info.st_mode);
}
bool file_exist(const std::string file)
{
	struct stat buffer;
	return (stat(file.c_str(),&buffer) == 0);
}
bool is_file(const std::string file)
{
	struct stat buffer;
	if (stat(file.c_str(),&buffer) != 0)
	{
		return false;
	}
	return S_ISREG(buffer.st_mode);
}
bool can_read(const std::string file)
{
	return (access(file.c_str(),R_OK) == 0);
}
bool can_write(const std::string file)
{
	return (access(file.c_str(),W_OK | X_OK) == 0);
}
bool read_file(const std::string& path, std::string& out)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();
    return true;
}

Block find_best_location(const std::string& path, const server& server_config)
{
	Block best;
	size_t max_len = 0;

	for (std::list<Block>::const_iterator it = server_config.get_srvLocations().begin();
		 it != server_config.get_srvLocations().end(); ++it)
	{
		std::string loc = it->getName();

		if (path.find(loc) == 0 && (path.length() == loc.length() || path[loc.length()] == '/') && loc.length() > max_len)
		{
			best = *it;
			max_len = loc.length();
		}
	}
	return best;
}

//Guarda lo que hay despues del . en la extension | Saca la extensión de un archivo (.php, .py)
std::string extractExtension(const std::string& path)
{
	size_t dotPos = path.find_last_of('.');

	if (dotPos == std::string::npos)
		return "";

	return path.substr(dotPos);
}
std::string take_parent_path(const std::string& chilf_path)
{
	size_t pos = chilf_path.rfind('/');
	std::string parent_path;
	if (pos == std::string::npos) 
		parent_path = "."; 
	else if (pos == 0)
		parent_path = "/"; 
	else
		parent_path = chilf_path.substr(0, pos);
	return parent_path;
}