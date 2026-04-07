/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:29:06 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/07 14:40:07 by pablalva         ###   ########.fr       */
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