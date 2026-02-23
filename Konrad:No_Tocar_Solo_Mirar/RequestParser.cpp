/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 19:08:00 by ksudyn            #+#    #+#             */
/*   Updated: 2026/02/23 21:10:49 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

void RequestParser::ParseBody(const std::string& bodyContent, Request& request)
{
    request.body =  bodyContent;
}

// Para eliminar espacios al principio y al final de un texto
// find_first_not_of(" ") busca el primer carácter que no sea espacio
// find_last_not_of(" ") busca el último carácter que no sea espacio
// Y el substr extrae el str sin espacios
std::string RequestParser::Trim(const std::string& str)
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
void RequestParser::ParseHeaders(const std::string& headersText, Request& request)
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
            std::string key = Trim(line.substr(0, colon));
            std::string value = Trim(line.substr(colon + 1));
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
void RequestParser::ParseRequestLine(const std::string& line, Request& request)
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


// Esta función recibe la request completa en texto plano.
// Ejemplo:
// "GET /test.py HTTP/1.1\r\nHost: localhost\r\n\r\nHola"

// Buscamos "\r\n\r\n" que es donde terminan los headers.
// find devuelve la posición donde empieza esa secuencia.
// Si no lo encuentra devuelve std::string::npos.

// Con substr(0, headerEnd) extraemos solo la parte de cabecera
// (request line + headers).

// Si encontró el final de cabecera,
// extraemos el body usando substr(headerEnd + 4).
// Sumamos 4 porque "\r\n\r\n" tiene 4 caracteres.

// Convertimos la parte de cabecera en un istringstream
// para poder leer línea por línea usando getline.

// La primera línea que leemos es la Request Line:
// "GET /test.py HTTP/1.1"
// La enviamos a ParseRequestLine para dividir:
// method, path, query, version.

// El resto de líneas son los headers.
// Los leemos uno por uno con getline.
// Si la línea termina en '\r', lo eliminamos.
// Vamos acumulando todas esas líneas en un string.

// Luego llamamos a ParseHeaders para convertir esas líneas
// en pares clave-valor dentro del map headers.

// Finalmente llamamos a ParseBody para guardar el body.

// La función devuelve el objeto Request completamente rellenado.
Request RequestParser::parse(const std::string& rawRequest)
{
    Request request;

    size_t headerEnd = rawRequest.find("\r\n\r\n");

    std::string headerPart = rawRequest.substr(0, headerEnd);
    std::string bodyPart;

    if (headerEnd != std::string::npos)
        bodyPart = rawRequest.substr(headerEnd + 4);

    std::istringstream stream(headerPart);
    std::string line;

    std::getline(stream, line);
    if (!line.empty() && line[line.size()-1] == '\r')
        line.erase(line.size()-1);

    ParseRequestLine(line, request);

    std::string headersOnly;
    while (std::getline(stream, line))
    {
        if (!line.empty() && line[line.size()-1] == '\r')
            line.erase(line.size()-1);

        headersOnly += line + "\n";
    }

    ParseHeaders(headersOnly, request);
    ParseBody(bodyPart, request);

    return request;
}


#include <iostream>

int main()
{
    // Simulamos una request HTTP completa
    std::string rawRequest =
        "POST /test.py?name=juan HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 11\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hola Mundo";

    RequestParser parser;
    Request req = parser.parse(rawRequest);

    // Mostramos resultados
    std::cout << "Method: " << req.method << std::endl;
    std::cout << "Path: " << req.path << std::endl;
    std::cout << "Query: " << req.query << std::endl;
    std::cout << "Version: " << req.version << std::endl;

    std::cout << "\nHeaders:\n";
    for (std::map<std::string, std::string>::iterator it = req.headers.begin();
         it != req.headers.end(); ++it)
    {
        std::cout << it->first << " => " << it->second << std::endl;
    }

    std::cout << "\nBody: " << req.body << std::endl;

    return 0;
}