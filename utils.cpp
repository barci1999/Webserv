/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:29:06 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/22 17:01:37 by pablalva         ###   ########.fr       */
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
	if (stat(file.c_str(), &buffer) == 0)
    {
        //std::cout << "[EXISTS OK] " << file << std::endl;
        return true;
    }
    else
    {
        //std::cout << "[EXISTS FAIL] " << file << std::endl;
        return false;
    }
}
bool is_file(const std::string file)
{
	struct stat buffer;
	if (stat(file.c_str(),&buffer) != 0)
	{
		//std::cout<<"NO SOY UN FILE"<<std::endl;
		return false;
	}
	return S_ISREG(buffer.st_mode);
}
bool can_read(const std::string file)
{
    if (access(file.c_str(), R_OK) == 0)
    {
        //std::cout << "[READ OK] " << file << std::endl;
        return true;
    }
    else
    {
        //std::cout << "[READ FAIL] " << file << std::endl;
        return false;
    }
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
static std::string normalize(const std::string& p)
{
    if (p.empty())
        return p;

    std::string res = p;

    while (res.size() > 1 && res[res.size() - 1] == '/')
        res.erase(res.size() - 1);

    return res;
}

Block find_best_location(const std::string& path, const server& server_config)
{
    Block best;
    size_t max_len = 0;
    bool found = false;

    std::string norm_path = normalize(path);

    const std::list<Block>& locs = server_config.get_srvLocations();

    for (std::list<Block>::const_iterator it = locs.begin();
         it != locs.end(); ++it)
    {
        std::string loc = normalize(it->getName());

        if (loc.empty())
            continue;

        if (norm_path.compare(0, loc.size(), loc) == 0 &&
            (norm_path.size() == loc.size() ||
             norm_path[loc.size()] == '/'))
        {
            if (!found || loc.size() > max_len)
            {
                best = *it;
                max_len = loc.size();
                found = true;
            }
        }
    }

    if (!found)
    {
        Block empty;
        empty.setName(""); // importante: estado explícito
        return empty;
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
bool is_valid_number(const std::string& nbr)
{
	if (nbr.empty())
        return false;

    for (size_t i = 0; i < nbr.size(); i++)
    {
        if (!std::isdigit(nbr[i]))
            return false;
    }
    return true;
	
}
std::string parse_chunked_body(Request& req)
{
    std::string old_body = req.get_body();
    std::string result;

    size_t i = 0;
    bool found_last_chunk = false;

    while (i < old_body.size())
    {
        size_t end_line = old_body.find("\r\n", i);
        if (end_line == std::string::npos)
            return "";

        std::string size_str = old_body.substr(i, end_line - i);

        char* endptr = NULL;
        long size = strtol(size_str.c_str(), &endptr, 16);

        if (*endptr != '\0' || size < 0)
            return "";

        i = end_line + 2;

        if (size == 0)
        {
            found_last_chunk = true;
            i += 2; // saltar el \r\n después del 0
            break;
        }

        if (i + (size_t)size > old_body.size())
            return "";

        result.append(old_body.substr(i, size));

        i += size;

        if (i + 1 >= old_body.size() || old_body.substr(i, 2) != "\r\n")
            return "";

        i += 2;
    }

    if (!found_last_chunk)
        return "";

    if (i != old_body.size())
        return "";

    return result;
}