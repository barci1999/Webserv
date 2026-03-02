/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 18:45:47 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/02 21:21:19 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIProcess.hpp"

//Guarda lo que hay despues del . en la extension
std::string CGIProcess::extractExtension(const std::string& path)
{
    size_t dotPos = path.find_last_of('.');

    if (dotPos == std::string::npos)
        return "";

    return path.substr(dotPos);
}

//Se busca cgi_extension y cgi_pass y se guarda en la variables
void CGIProcess::extractCGIConfig(const Block& location)
{
    const std::vector<Directive>& dirs = location.getDirectives();

    _cgiExtension = "";
    _cgiPass = "";

    for (size_t i = 0; i < dirs.size(); i++)
    {
        if (dirs[i].name == "cgi_extension" && !dirs[i].args.empty())
            _cgiExtension = dirs[i].args[0];

        if (dirs[i].name == "cgi_pass" && !dirs[i].args.empty())
            _cgiPass = dirs[i].args[0];
    }
}

bool CGIProcess::isCGI(const Request& request, const Block& location)
{
    extractCGIConfig(location);

    if (_cgiExtension.empty() || _cgiPass.empty())
        return false;

    std::string extension = extractExtension(request.path);

    if (extension == _cgiExtension)
    {
        _scriptPath = request.path;
        return true;
    }

    return false;
}