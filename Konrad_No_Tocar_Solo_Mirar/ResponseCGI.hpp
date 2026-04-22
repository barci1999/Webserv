/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 15:02:17 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/22 15:22:21 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CGIProcess.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "../Parseo_solo_toca_Pablo/server.hpp"

#include <vector>

//QUE TIENE QUE VER LA SQUERY CON EL CGI BUSCAR, POR QUE PABLO NO LAS USA

std::vector<Response> CGI_Response(std::vector<Request>& requests, server& srv);
Response handleRequest(Request& req, server& srv);