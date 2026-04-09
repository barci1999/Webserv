/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_cgi_test.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 16:30:12 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/09 18:53:30 by ksudyn           ###   ########.fr       */
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

#include "Parseo_solo_toca_Pablo/server.hpp"
#include "Parseo_solo_toca_Pablo/Block.hpp"
#include "Parseo_solo_toca_Pablo/Directive.hpp"

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
}

int main()
{
    Request req1, req2;
    createRequests(req1, req2);

    server srv = createServer();

    // =========================================================
    // 🔴 MODO 1: SECUENCIAL (SIN POLL)
    // =========================================================


    std::cout << "\n===== SECUENCIAL =====\n";

    CGIProcess cgi1;
    if (!cgi1.isCGI(req1, srv))
        return 1;

    cgi1.execute(req1, srv);

    while (!cgi1.isFinished())
        cgi1.readFromPipe();

    std::cout << "\n--- CGI 1 ---\n";
    std::cout << cgi1.getBuffer() << std::endl;


    CGIProcess cgi2;
    if (!cgi2.isCGI(req2, srv))
        return 1;

    cgi2.execute(req2, srv);

    while (!cgi2.isFinished())
        cgi2.readFromPipe();

    std::cout << "\n--- CGI 2 ---\n";
    std::cout << cgi2.getBuffer() << std::endl;


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

    std::cout << "\n🔥 TODO TERMINADO\n";

    return 0;
}
