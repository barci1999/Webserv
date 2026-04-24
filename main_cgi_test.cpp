/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cgi_test.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 16:30:12 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/24 20:23:40 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <map>

#include "Konrad_No_Tocar_Solo_Mirar/CGIProcess.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Request.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/RequestParser.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Response.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/ResponseCGI.hpp"

#include "Parseo_solo_toca_Pablo/server.hpp"
#include "Parseo_solo_toca_Pablo/Block.hpp"
#include "Parseo_solo_toca_Pablo/Directive.hpp"
#include "Parseo_solo_toca_Pablo/Parser.hpp"


// 🔧 función común para crear config
server createServer()
{
    server srv;

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

    std::list<Block> locations;
    locations.push_back(location);

    srv.insert_locations(locations);

    return srv;
}

// 🔧 función común para crear requests
void createRequests(Request &req1, Request &req2)
{
    std::string raw_request1 =
        "POST /cgi/test.py HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 11\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello World";

    std::string raw_request2 =
        "POST /cgi/slow.py HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 5\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hola!";

    RequestParser::parse(raw_request1, req1);
    RequestParser::parse(raw_request2, req2);
    RequestParser::valid_request(req1);
    RequestParser::valid_request(req2);
}

Response executeCGIWithPoll(Request& req, server& srv)
{
    CGIProcess* cgi = new CGIProcess();

    if (!cgi->isCGI(req, srv))
        throw std::runtime_error("No es CGI");

    cgi->execute(req, srv);

    int fd = cgi->getFD();

    struct pollfd pol;
    pol.fd = fd;
    pol.events = POLLIN | POLLHUP;
    pol.revents = 0;

    std::vector<pollfd> pollfds;
    pollfds.push_back(pol);

    while (true)
    {
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0)
        {
            perror("poll");
            break;
        }

        // 🔴 CGI TERMINADO
        if (pollfds[0].revents & (POLLHUP | POLLERR))
        {
            cgi->readFromPipe(); // leer lo último

            Response res = cgi->buildResponse();

            close(fd);
            delete cgi;

            return res;
        }

        // 🟢 DATOS DISPONIBLES
        if (pollfds[0].revents & POLLIN)
        {
            cgi->readFromPipe();
        }
    }

    delete cgi;
    throw std::runtime_error("Error ejecutando CGI");
}






int main()
{
    Request req1, req2;
    createRequests(req1, req2);

    Parser parser;
	std::vector<server> list = parser.parseFile("conf_server_cgi.txt");
	std::vector<server>::iterator it = list.begin();
	server Server = *it;
    std::cout<<Server<<std::endl;

    // =========================================================
    // 🔴 MODO 1: SECUENCIAL (SIN POLL)
    // =========================================================


    // std::cout << "\n===== SECUENCIAL =====\n";

    // CGIProcess cgi1;
    // if (!cgi1.isCGI(req1, srv))
    //     return 1;

    // cgi1.execute(req1, srv);

    // while (!cgi1.isFinished())
    //     cgi1.readFromPipe();

    // std::cout << "\n--- CGI 1 ---\n";
    // std::cout << cgi1.getBuffer() << std::endl;


    // CGIProcess cgi2;
    // if (!cgi2.isCGI(req2, srv))
    //     return 1;

    // cgi2.execute(req2, srv);

    // while (!cgi2.isFinished())
    //     cgi2.readFromPipe();

    // std::cout << "\n--- CGI 2 ---\n";
    // std::cout << cgi2.getBuffer() << std::endl;


    // =========================================================
    // 🟢 MODO 2: CON POLL (PARALELO)
    // =========================================================

    // std::cout << "\n===== CON POLL =====\n";


    // // 🔹 POLL STRUCTURES
    // std::vector<struct pollfd> pollfds;
    // std::map<int, CGIProcess*> cgi_map;

    // // 🔹 LANZAR CGI 1
    // CGIProcess* pcgi1 = new CGIProcess();
    // if (pcgi1->isCGI(req1, srv))
    // {
    //     pcgi1->execute(req1, srv);
    //     int fd = pcgi1->getFD();

    //     struct pollfd p;
    //     p.fd = fd;
    //     p.events = POLLIN | POLLHUP;
    //     p.revents = 0;

    //     pollfds.push_back(p);
    //     cgi_map[fd] = pcgi1;
    // }

    // // 🔹 LANZAR CGI 2
    // CGIProcess* pcgi2 = new CGIProcess();
    // if (pcgi2->isCGI(req2, srv))
    // {
    //     pcgi2->execute(req2, srv);
    //     int fd = pcgi2->getFD();

    //     struct pollfd p;
    //     p.fd = fd;
    //     p.events = POLLIN | POLLHUP;
    //     p.revents = 0;

    //     pollfds.push_back(p);
    //     cgi_map[fd] = pcgi2;
    // }

    // // 🔥 LOOP PRINCIPAL
    // while (!cgi_map.empty())
    // {
    //     int ret = poll(&pollfds[0], pollfds.size(), -1);
    //     if (ret < 0)
    //     {
    //         perror("poll");
    //         break;
    //     }

    //     for (size_t i = 0; i < pollfds.size(); i++)
    //     {
    //         int fd = pollfds[i].fd;

    //         // 🔹 ERROR o cierre
    //         if (pollfds[i].revents & (POLLERR | POLLHUP))
    //         {
    //             CGIProcess* cgi = cgi_map[fd];

    //             // 🔥 leer lo último antes de cerrar
    //             cgi->readFromPipe();

    //             std::cout << "\n--- CGI FINISHED (HUP) ---\n";
    //             std::cout << cgi->getBuffer() << std::endl;

    //             Response res = cgi->buildResponse();

    //             std::cout << "\n--- RESPONSE ---\n";
    //             std::cout << res.get_body() << std::endl;

    //             close(fd);
    //             delete cgi;

    //             cgi_map.erase(fd);
    //             pollfds.erase(pollfds.begin() + i);
    //             i--;
    //             continue;
    //         }

    //         // 🔹 HAY DATOS
    //         if (pollfds[i].revents & POLLIN)
    //         {
    //             CGIProcess* cgi = cgi_map[fd];
    //             cgi->readFromPipe();
    //         }
    //     }
    // }

    // std::cout << "\n===== CON POLL ( HECHO PRIMERO UNO Y LUEGO EL OTRO) =====\n";

    // try
    // {
    //     Response res1 = executeCGIWithPoll(req1, srv);
    //     std::cout << "\n--- RESPONSE 1 ---\n";
    //     std::cout << res1.get_body() << std::endl;

    //     Response res2 = executeCGIWithPoll(req2, srv);
    //     std::cout << "\n--- RESPONSE 2 ---\n";
    //     std::cout << res2.get_body() << std::endl;
    //     // executeCGIWithPoll()  // uno
    //     // executeCGIWithPoll()  // otro
    //     // Eso crea 2 loops de poll independientes, nunca conviven, nunca se ejecutan a la vez
    // }
    // catch (std::exception& e)
    // {
    //     std::cerr << "Error: " << e.what() << std::endl;
    // }

    

    std::cout << "\n===== CON POLL ( EN PRINCIPIO BIEN HECHO) =====\n";

    std::vector<Request> requests;
    requests.push_back(req1);
    requests.push_back(req2);
    for (std::vector<Request>::iterator it = requests.begin() ; it  != requests.end();++it )
    {
        std::cout<< *it<<std::endl;
    }
    

    std::cout << "\n===== MULTI CGI (REAL PARALELO) =====\n";

    std::vector<Response> responses =
        CGI_Response(requests, Server);

    for (size_t i = 0; i < responses.size(); i++)
    {
        std::cout << "\n--- RESPONSE " << i << " ---\n";
        //std::cout << responses[i].get_body() << std::endl;
        
        std::cout << " ESTOY AQUIIIIIIIIIIIIIIIIIII\n" << responses[i].toHTTPString() << std::endl;
    }
    

    std::cout << "\n🔥 TODO TERMINADO\n";

    return 0;
}
