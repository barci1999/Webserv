/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 15:21:24 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/06 16:22:41 by pablalva         ###   ########.fr       */
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
    CGIProcess* cgi = new CGIProcess();

    if (!cgi->isCGI(request, serv))
    {
        delete cgi;
        return;
    }
    
    // 1. execute ya añade los FDs al vector pollFds (según vimos antes)
    cgi->execute(request, serv, pollFds);

    // 2. Registrar el pipe de LECTURA (Salida del CGI -> Entrada del Server)
    int out_fd = cgi->getFD(); 
    cgi_map[out_fd] = cgi;
    cgi_to_client[out_fd] = client_fd;

    // 3. Registrar el pipe de ESCRITURA (Solo si es POST)
    int in_fd = cgi->getInputFd();
	if (out_fd != -1) {
        cgi_map[out_fd] = cgi;
        cgi_to_client[out_fd] = client_fd;
        std::cout << "MAP: Guardado out_fd " << out_fd << std::endl;
    }
    if (in_fd != -1) {
        cgi_map[in_fd] = cgi;
        cgi_to_client[in_fd] = client_fd;
        std::cout << "MAP: Guardado in_fd " << in_fd << std::endl;
    }
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
		for (size_t i = 0; i < pollFds.size(); i++) {
        if (pollFds[i].fd == client_fd) {
            pollFds[i].events = 0; // Deja de escuchar al cliente mientras el CGI trabaja
            break;
        }
    }
        CGI_Response(request, serv, pollFds, cgi_map, cgi_to_client, client_fd);
        return false; // aqui sún no hay respuesta
    }//Un CGI NO devuelve respuesta…, devuelve un evento futuro

    response = Response(request, serv);
    return true;
}
