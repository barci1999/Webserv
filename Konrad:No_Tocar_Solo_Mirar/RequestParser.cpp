/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 19:08:00 by ksudyn            #+#    #+#             */
/*   Updated: 2026/02/20 20:49:02 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

void RequestParser::parseBody(const std::string& bodyContent, Request& request)
{
    request.body =  bodyContent;
}

// Para eliminar espacios al principio y al final de un texto
// find_first_not_of(" ") busca el primer carácter que no sea espacio
// find_last_not_of(" ") busca el último carácter que no sea espacio
// Y el substr extrae el str sin espacios
std::string RequestParser::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" ");
    size_t end = str.find_last_not_of(" ");

    if(start ==  std::string::npos)
        return "";
    
        return str.substr(start, end -start +1);
}


// istringstream lee un string como si fuera un archivo, sin esto se tenfria que buscar el \n, cortar substring
// divides asi el headersText y lo metes en stream para usar el getine y trabajar linea por linea.
// find busca la posicion de lo que le pases, en este caso : dentro de line y lo guardamos en colon
// extraemos la clave - valor y guardamos dentro de key y value entre lo que hay antes y despies de : y con trim quitamos espacios
// Y con headers 
void RequestParser::parseHeaders(const std::string& headersText, Request& request)
{
    std::istringstream stream(headersText);
    std::string line;

    while (std::getline(stream, line))
    {
        if (line.empty())
            continue;

        size_t colon = line.find(':');
        if (colon != std::string::npos)
        {
            std::string key = trim(line.substr(0, colon));
            std::string value = trim(line.substr(colon + 1));
            request.headers[key] = value;
        }
    }
}

// Convertimos la línea completa "GET /test.py?name=juan HTTP/1.1" en un flujo para poder leer palabra por palabra.
//El operador >> lee desde la posición actual del stream, ignora los espacios iniciales, y guarda en la variable todos los caracteres hasta el siguiente espacio
// Buscamos si la URL contiene un '?', find devuelve la posición donde está el carácter, si no se encuentra devuelve std::string::npos.
//  SI HAY '?' se extrae desde el inicio hasta antes del '?' y eso seria el PATH puro
// Luego s extrae desde la posicion del '?' hasta el final y eso es la QUERY
// Si no hay '?', toda la URL es solo path.
void RequestParser::parseRequestLine(const std::string& line, Request& request)
{
    std::istringstream stream(line);
    stream >> request.method;
    std::string fullPath;
    stream >> fullPath;
    stream >> request.version;

    size_t queryPos = fullPath.find('?');

    if (queryPos != std::string::npos)
    {
        request.path = fullPath.substr(0, queryPos);
        request.query = fullPath.substr(queryPos + 1);
    }
    else
    {
        request.path = fullPath;
        request.query = "";
    }
}


Request RequestParser::parse(const std::string& rawRequest)
{
    
}