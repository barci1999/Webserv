/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 18:45:47 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/09 21:15:22 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIProcess.hpp"
#include "server.hpp"

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

    Directive root = server::check_directives("root",location);
    std::string locationPrefix = location.getName(); 
    std::string relativePath = request.path.substr(locationPrefix.length());

    std::string rootPath = root.args[0];
    
    if (rootPath[rootPath.size() - 1] == '/')
        return rootPath + relativePath;

    return rootPath + "/" + relativePath;
}
//Si por algun casual el root ya termina en / no se añade uno.
//Al final se guarda _fullPath = buildFullPath(request, location); en la funcion que se llame.


std::string CGIProcess::serveStaticFile(const Request& request, const Block& location)
{
    std::string fullPath = buildFullPath(request, location);//se construye la ruta real del archivo

    std::ifstream file(fullPath.c_str());

    if (!file.is_open())
        return "404 Not Found";

    std::stringstream buffer;
    buffer << file.rdbuf();//esto te devuelve todo el contenido del archivo y se copia con << dentro de buffer

    return buffer.str();
}

std::string CGIProcess::execute(const Request& request, const Block& location)
{
    _fullPath = buildFullPath(request, location);

    std::ifstream file(_fullPath.c_str());

    if (!file.is_open())
        return "404 CGI script not found";

    return "CGI execution placeholder";
}

//Esta funcion decide que hacer en base a si es CGI o no
std::string CGIProcess::handleRequest(Request& request, Block& location)
{
    CGIProcess cgi;

    if (cgi.isCGI(request, location))
    {
        return execute(request, location);
    }
    else
    {
        return serveStaticFile(request, location);
    }
}