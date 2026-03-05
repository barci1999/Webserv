/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 18:45:47 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/05 15:33:24 by ksudyn           ###   ########.fr       */
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

//Se busca cgi_extension y cgi_pass y se guarda en la variables, al principio las inicializo vacias
//Aqui buscamos dentro de cada location esas variables y si existen y no estan vacias, guardamos el primer argumento
void CGIProcess::extractCGIConfig(const Block& location)
{
    const std::vector<Directive>& vect_directie = location.getDirectives();

    _cgiExtension = "";
    _cgiPass = "";

    for (size_t i = 0; i < vect_directie.size(); i++)
    {
        if (vect_directie[i].name == "cgi_extension" && !vect_directie[i].args.empty())
            _cgiExtension = vect_directie[i].args[0];

        if (vect_directie[i].name == "cgi_pass" && !vect_directie[i].args.empty())
            _cgiPass = vect_directie[i].args[0];
    }
}

// Aquí verifico que si el contenido del request es el mismo que la extension del location
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

//CREAMOS EL FULLPATH

std::string CGIProcess::buildFullPath(const Request& request, const Block& location)
{
    std::string root;

    const std::vector<Directive>& dirs = location.getDirectives();

    for (size_t i = 0; i < dirs.size(); i++)
    {
        if (dirs[i].name == "root" && !dirs[i].args.empty())
            root = dirs[i].args[0];
    }
    Directive hola = location.ch
    std::string locationPrefix = location.getName(); 
    std::string relativePath = request.path.substr(locationPrefix.length());

    return root + "/" + relativePath;
}
//Al final se guarda _fullPath = buildFullPath(request, location); en la funcion que se llame.

