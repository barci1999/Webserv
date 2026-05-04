/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 15:02:17 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/04 18:00:47 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CGIProcess.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "../Parseo_solo_toca_Pablo/server.hpp"
#include "../pollLoop.hpp"
#include <vector>

//QUE TIENE QUE VER LA SQUERY CON EL CGI BUSCAR, POR QUE PABLO NO LAS USA

bool handleRequest(Request& request,server& serv,std::vector<pollfd>& pollFds,
    std::map<int, CGIProcess*>& cgi_map, std::map<int, int>& cgi_to_client, int client_fd, Response& response);

void CGI_Response(Request& request, server& serv, std::vector<pollfd>& pollFds,
    std::map<int, CGIProcess*>& cgi_map, std::map<int, int>& cgi_to_client, int client_fd);