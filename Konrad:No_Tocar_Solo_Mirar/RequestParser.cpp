/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 19:08:00 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/22 21:26:39 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"
#include "cstdlib"

void RequestParser::ParseBody(const std::string& bodyContent, Request& request)
{
	request.set_body(bodyContent);
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
	
	return str.substr(start, end -start + 1);
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
			if (key.empty() || value.empty())
			{
				RequestParser::set_error(request,400,"Bad Request");
				return;
			}
			for (size_t i = 0; i < key.size(); ++i)
			{
				if (key[i] == '\n' || key[i] == '\r')
				{
					RequestParser::set_error(request,400,"Bad Request");
					return;
				}
			}
			for (size_t i = 0; i< value.size(); ++i)
			{
				if (value[i] == '\n' || value[i] == '\r')
				{
					RequestParser::set_error(request,400,"Bad Request");
					return;
				}
			}
			for (size_t i = 0; i < key.size(); i++)
				key[i] = std::tolower(key[i]);
			if(key == "content-length")
			{
				for (size_t i = 0; value[i]; i++)
				{
					if (!std::isdigit(value[i]))
					{
						RequestParser::set_error(request,400,"Bad Request");
						return;
					}
				}
			}
			request.set_a_header(key,value);
		}
	}
}

// Convertimos la línea completa "GET /test.py?name=juan HTTP/1.1" en un flujo para poder leer palabra por palabra.
//El operador >> lee desde la posición actual del stream, ignora los espacios iniciales, y guarda en la variable todos los caracteres hasta el siguiente espacio
// Buscamos si la URL contiene un '?', find devuelve la posición donde está el carácter, si no se encuentra devuelve std::string::npos.
//  SI HAY '?' se extrae desde el inicio hasta antes del '?' y eso seria el PATH puro
// Luego s extrae desde la posicion del '?' hasta el final y eso es la QUERY
// Si no hay '?', toda la URL es solo path.
// static void imprimirEscapado(const std::string& s) {
// 	for (size_t i = 0; i < s.size(); i++) {
// 		switch (s[i]) {
// 			case '\n': std::cout << "\\n"; break;
// 			case '\t': std::cout << "\\t"; break;
// 			case '\r': std::cout << "\\r"; break;
// 			case '\b': std::cout << "\\b"; break;
// 			case '\f': std::cout << "\\f"; break;
// 			case '\v': std::cout << "\\v"; break;
// 			case '\\': std::cout << "\\\\"; break;
// 			default:
// 				std::cout << s[i];
// 		}
// 	}
// 	std::cout<<std::endl;
// }
void RequestParser::ParseRequestLine(const std::string& line, Request& request)
{
	//imprimirEscapado(line);
	std::istringstream stream(line);
	std::string temp;
	stream >> temp;
	request.set_method(temp);
	std::string fullPath;
	stream >> fullPath;
	stream >> temp;
	request.set_version(temp);

	size_t queryPos = fullPath.find('?');

	if (queryPos != std::string::npos)
	{
		request.set_path(fullPath.substr(0, queryPos));
		request.set_query(fullPath.substr(queryPos + 1));
	}
	else
	{
		request.set_path(fullPath);
		request.set_query("");
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
Request &RequestParser::parse(const std::string& rawRequest,Request &request)
{

	size_t headerEnd = rawRequest.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
	{
		RequestParser::set_error(request,(unsigned int)400,"Bad Request");
		return request;
	}
	

	std::string headerPart = rawRequest.substr(0, headerEnd);
	std::string bodyPart;
	if (headerEnd != std::string::npos)
		bodyPart = rawRequest.substr(headerEnd + 4);
	
	std::istringstream stream(headerPart);
	std::string line;
	
	std::getline(stream, line);
	size_t i = line.find("\r");
	if(i == std::string::npos)
	{
		RequestParser::set_error(request,(unsigned int)400,"Bad Request");
		return request;
	}
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


// #include <iostream>

// int main()
// {
//     // Simulamos una request HTTP completa
//     std::string rawRequest =
//         "POST /test.py?name=juan HTTP/1.1\r\n"
//         "Host: localhost\r\n"
//         "Content-Length: 11\r\n"
//         "Content-Type: text/plain\r\n"
//         "\r\n"
//         "Hola Mundo";

//     RequestParser parser;
//     Request req = parser.parse(rawRequest);

//     // Mostramos resultados
//     std::cout << "Method: " << req.get_method() << std::endl;
//     std::cout << "Path: " << req.get_path() << std::endl;
//     std::cout << "Query: " << req.get_query() << std::endl;
//     std::cout << "Version: " << req.get_version() << std::endl;

//     std::cout << "\nHeaders:\n";
//     for (std::map<std::string, std::string>::iterator it = req.get_headers().begin(); it != req.get_headers().end(); ++it)
//     {
//         std::cout << it->first << " => " << it->second << std::endl;
//     }

//     std::cout << "\nBody: " << req.get_body() << std::endl;

//     return 0;
// }
void RequestParser::set_error(Request& req, unsigned int code,const std::string error_phrase)
{
	req.set_final_status(error_phrase);
	req.set_status_code(code);
}
bool RequestParser::valid_method( Request& to_check)
{
	std::string temp = to_check.get_method();
	if (temp ==  "GET" || temp == "DELETE" || temp == "POST")
	{
		return true;
	}
	RequestParser::set_error(to_check,405,"Method Not Allowed");
	return false;
}
bool RequestParser::valid_path(Request& to_check)
{
	std::string temp = to_check.get_path();

	if (temp.empty() || temp[0] != '/')
	{
		RequestParser::set_error(to_check,400,"Bad Request");
		return false;
	}
	if (temp.length() > 2048)
	{
		RequestParser::set_error(to_check,414,"URI Too Long");
		return false;
	}
	if (temp.find(' ') != std::string::npos || temp.find('#') != std::string::npos)
	{
		RequestParser::set_error(to_check,400,"Bad Request");
		return false;
	}
	for (size_t i = 0; i < temp.size(); i++)
	{
		unsigned char c = temp[i];
		if (c <= 31 || c == 127)
		{
			RequestParser::set_error(to_check,400,"Bad Request");
			return false;
		}
	}
	return true;
}
bool RequestParser::valid_query(Request& to_check)
{
	const std::string& query = to_check.get_query();
	if (query.length() > 2048)
	{
		RequestParser::set_error(to_check,414,"URI Too Long");
		return false;
	}
	for (size_t i = 0; i < query.size(); ++i)
	{
		unsigned char c = query[i];
		if (c <= 31 || c == 127)
		{
			RequestParser::set_error(to_check,400,"Bad Request");
			return false;
		}
	}
	return true;
}
bool RequestParser::valid_version(Request& to_check)
{
	std::string temp = to_check.get_version();
	if (temp != "HTTP/1.1")
	{
		RequestParser::set_error(to_check,505,"HTTP Version Not Supported");
		return false;
	}
	return true;
	
}
bool RequestParser::valid_headers(Request& to_check)
{
	const std::map<std::string,std::string>& headers = to_check.get_headers();
	if (headers.find("host") == headers.end())
	{
		
		RequestParser::set_error(to_check,400,"Bad Request");
		return false;
	}
	if (to_check.get_method() == "POST")
	{
		if (headers.find("content-length") == headers.end())
		{
			RequestParser::set_error(to_check,400,"Bad Request");
			return false;
		}
	}
	for (std::map<std::string,std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		const std::string& name = it->first;
		for (size_t i = 0; i < name.size(); ++i)
		{
			unsigned char c = name[i];
			if (c <= 31 || c == 127)
			{
				RequestParser::set_error(to_check,400,"Bad Request");
				return false;
			}
		}
	}
	return true;
}
bool RequestParser::valid_body(Request& to_check)
{
	std::string method = to_check.get_method();
	std::string body = to_check.get_body();
	std::map<std::string,std::string> headers =  to_check.get_headers();
	if (method != "POST")
	{
		return true;
	}
	std::map<std::string,std::string>::iterator it = headers.find("content-length");
	if(it == headers.end())
	{
		RequestParser::set_error(to_check,400,"Bad Request");
		return false;
	}
	if (it->second.empty())
	{
		RequestParser::set_error(to_check,400,"Bad Request");
		return false;
	}
	size_t content_length = std::atoi(it->second.c_str());
	if (body.length() != content_length)
	{
		RequestParser::set_error(to_check,400,"Bad Request");
		return false;
	}
	return true;
}



bool RequestParser::valid_request(Request& to_check)
{
	typedef bool (*validator)(Request&);

	if (to_check.get_status_code() != 200 && to_check.get_status_code() != 0)
	{
		std::cout << to_check.get_status_code()<<std::endl;
		return false;
	}
	validator validators[] = {
		valid_method,
		valid_path,
		valid_query,
		valid_version,
		valid_headers,
		valid_body
	};

	size_t count = sizeof(validators) / sizeof(validators[0]);
	

	for (size_t i = 0; i < count; ++i)
	{
		if (!validators[i](to_check))
		{
			return false;
		}
	}
	to_check.set_final_status("OK");
	to_check.set_status_code(200);
	return true;
}