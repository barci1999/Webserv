/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 15:21:24 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/05 11:48:13 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ResponseCGI.hpp"
#include <map>
#include <iostream>
#include <unistd.h>
#include <poll.h>

#include "ResponseCGI.hpp"
#include <map>
#include <iostream>
#include <unistd.h>
#include <poll.h>

void CGI_Response(Request& request, server& serv, std::vector<pollfd>& pollFds,
    std::map<int, CGIProcess*>& cgi_map, std::map<int, int>& cgi_to_client, int client_fd)
{
    // 🔹 Crear proceso CGI
    CGIProcess* cgi = new CGIProcess();

    // 🔴 Seguridad: comprobar otra vez
    if (!cgi->isCGI(request, serv))
    {
        delete cgi;
        return;
    }
    
    // 🔹 Ejecutar CGI (fork + exec)
    cgi->execute(request, serv,pollFds);

    int fd = cgi->getFD();

    // 🔹 Añadir a poll
    pollfd poll;
    poll.fd = fd;
    poll.events = POLLIN | POLLHUP;
    poll.revents = 0;

    pollFds.push_back(poll);

    // 🔹 Guardar relaciones
    cgi_map[fd] = cgi;            // fd → objeto CGI
    cgi_to_client[fd] = client_fd; // fd → cliente
}

// Esta función:

// Crea el CGI
// Hace fork + execve
// Obtiene el pipe de salida
// Lo mete en poll()
// Guarda quién es quién

// Y TERMINA

// NO hace poll()
// NO espera resultado
// NO devuelve Response
//Solo dice: “oye pollLoop, vigila este fd”


//FUNCION QUE NO VA AQUI.
//AQUI VERIFICO SI ES CGI Y SI LO ES, EJECUTO LO DEL CGI CON POLL Y SI NO RETORNO EL Response( req, srv);
bool handleRequest(Request& request,server& serv,std::vector<pollfd>& pollFds,
    std::map<int, CGIProcess*>& cgi_map, std::map<int, int>& cgi_to_client, int client_fd, Response& response)
{
    CGIProcess tmp;

    if (tmp.isCGI(request, serv))
    {
        CGI_Response(request, serv, pollFds, cgi_map, cgi_to_client, client_fd);
        return false; // aqui sún no hay respuesta
    }//Un CGI NO devuelve respuesta…, devuelve un evento futuro

    response = Response(request, serv);
    return true;
}
