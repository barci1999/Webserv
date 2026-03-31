/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cgi_test.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 16:30:12 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/31 16:44:58 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include "Konrad_No_Tocar_Solo_Mirar/CGIProcess.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Request.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/RequestParser.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Response.hpp"
#include "Parseo_solo_toca_Pablo/Block.hpp"
#include "Parseo_solo_toca_Pablo/Directive.hpp"

int main()
{
    // 🔹 1. Crear request CGI (GET o POST)
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

    // 🔹 2. Config fake (location CGI)
    Block location;
    location.setName("/cgi");

    Directive root;
    root.name = "root";
    root.args.push_back("test/cgi-bin"); // ⚠️ carpeta donde pondrás el script

    location.addDirective(root);

    // 🔹 3. Ejecutar CGI
    CGIProcess cgi;
    cgi.execute(req, location);

    // 🔹 4. Leer hasta que termine
    while (!cgi.isFinished())
    {
        cgi.readFromPipe();
        usleep(10000); // 10ms → evita busy loop
    }

    std::cout << "\n==== CGI RAW OUTPUT ====\n";
    std::cout << cgi.getBuffer() << std::endl;

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