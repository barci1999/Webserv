/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 19:08:00 by ksudyn            #+#    #+#             */
/*   Updated: 2026/02/19 21:26:44 by ksudyn           ###   ########.fr       */
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

Request RequestParser::parse(const std::string& rawRequest)
{
    
}