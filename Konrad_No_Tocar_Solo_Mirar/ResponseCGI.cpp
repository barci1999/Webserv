/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 15:21:24 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/03 20:14:42 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ResponseCGI.hpp"
#include <map>
#include <iostream>
#include <unistd.h>
#include <poll.h>


std::string Response::toHTTPString() const
{
    std::ostringstream oss;

    oss << "HTTP/1.1 " << _statusCode << " " << _reasonPhrase << "\r\n";

    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end(); ++it)
    {
        oss << it->first << ": " << it->second << "\r\n";
    }

    oss << "\r\n";
    oss << _body;

    return oss.str();
}

std::vector<Response> CGI_Response(std::vector<Request>& requests, server& srv)
{
    std::vector<Response> responses;

    std::vector<pollfd> pollfds;
    std::map<int, CGIProcess*> cgi_map;

    // 🔹 1. LANZAR TODOS LOS CGI
    for (size_t i = 0; i < requests.size(); i++)
    {
        CGIProcess* cgi = new CGIProcess();

        if (!cgi->isCGI(requests[i], srv))
        {
            delete cgi;
            continue;
        }

        cgi->execute(requests[i], srv);

        int fd = cgi->getFD();

        struct pollfd p;
        p.fd = fd;
        p.events = POLLIN | POLLHUP;
        p.revents = 0;

        pollfds.push_back(p);
        cgi_map[fd] = cgi;
    }

    // 🔥 2. LOOP ÚNICO (LA CLAVE)!
    while (!cgi_map.empty())
    {
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        if (ret < 0)
        {
            perror("poll");
            break;
        }

        for (size_t i = 0; i < pollfds.size(); i++)
        {
            int fd = pollfds[i].fd;

            // 🔴 TERMINADO
            if (pollfds[i].revents & (POLLHUP | POLLERR))
            {
                CGIProcess* cgi = cgi_map[fd];

                cgi->readFromPipe(); // leer lo último

                Response res = cgi->buildResponse();
                responses.push_back(res);

                close(fd);
                delete cgi;

                cgi_map.erase(fd);
                pollfds.erase(pollfds.begin() + i);
                i--;
                continue;
            }

            // 🟢 HAY DATOS
            if (pollfds[i].revents & POLLIN)
            {
                CGIProcess* cgi = cgi_map[fd];
                cgi->readFromPipe();
            }
        }
    }

    return responses;
}



//FUNCION QUE NO VA AQUI.
//AQUI VERIFICO SI ES CGI Y SI LO ES, EJECUTO LO DEL CGI CON POLL Y SI NO RETORNO EL Response( req, srv);
Response handleRequest(Request& req, server& srv)
{
    CGIProcess tmp;

    // 🔴 1. ¿ES CGI?
    if (tmp.isCGI(req, srv))
    {
        std::vector<Request> reqs;
        reqs.push_back(req);
        std::vector<Response> resps = CGI_Response(reqs, srv);

        if (!resps.empty())
            return resps[0];

        // error CGI
        Response error;
        error.set_statuscode(500);
        error.set_reasonphrase("Internal Server Error");
        error.set_body("CGI failed");
        error.set_error(error,500,srv);
        return error;
    }

    // 🔵 2. NO CGI → usar el de tu compañero
    return Response(req, srv);
}