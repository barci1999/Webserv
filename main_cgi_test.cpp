/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cgi_test.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 16:30:12 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/08 16:27:17 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>

#include "Konrad_No_Tocar_Solo_Mirar/CGIProcess.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Request.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/RequestParser.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Response.hpp"

#include "Parseo_solo_toca_Pablo/server.hpp"
#include "Parseo_solo_toca_Pablo/Block.hpp"
#include "Parseo_solo_toca_Pablo/Directive.hpp"

int main()
{
    // 🔹 1. Crear request CGI
    std::string raw_request =
        "POST /cgi/test.py HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 11\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello World";

    Request req;
    RequestParser::parse(raw_request, req);

    if (!RequestParser::valid_request(req))
    {
        std::cout << "Request inválida\n";
        return 1;
    }

    std::cout << "==== REQUEST ====\n";
    std::cout << req << std::endl;

    // 🔹 2. Crear SERVER
    server srv;

    // 🔸 LOCATION
    Block location;
    location.setName("/cgi");

    Directive root;
    root.name = "root";
    root.args.push_back("/sgoinfre/students/ksudyn/GITHUB/Webserv/test/cgi-bin");

    Directive ext;
    ext.name = "cgi_extension";
    ext.args.push_back(".py");

    Directive pass;
    pass.name = "cgi_pass";
    pass.args.push_back("/usr/bin/python3");

    location.addDirective(root);
    location.addDirective(ext);
    location.addDirective(pass);

    // 🔸 Meter location en lista
    std::list<Block> locations;
    locations.push_back(location);

    // 🔸 Insertar en server
    try
    {
        srv.insert_locations(locations);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error en server config: " << e.what() << std::endl;
        return 1;
    }

    // 🔹 3. Ejecutar CGI
    CGIProcess cgi;

    if (!cgi.isCGI(req, srv))
    {
        std::cout << "No es CGI\n";
        return 1;
    }

    cgi.execute(req, srv);
    
    // 🔹 4. Leer salida
    while (!cgi.isFinished())
    {

        cgi.readFromPipe();
        usleep(10000);
    }
    cgi.readFromPipe();  // 🔥 leer lo último que quede

    std::cout << "\n==== CGI RAW OUTPUT ====\n";
    std::cout << cgi.getBuffer() << std::endl;

    // 🔹 5. Parsear
    Response res = cgi.buildResponse();

    std::cout << "\n==== PARSED RESPONSE ====\n";
    std::cout << "Status: " << res.get_statusCode() << " "
              << res.get_reasonPhrase() << std::endl;

    std::cout << "\nHeaders:\n";
    std::map<std::string, std::string> headers = res.get_headers();
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }

    std::cout << "\nBody:\n";
    std::cout << res.get_body() << std::endl;

    return 0;
}
