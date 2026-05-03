/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 18:28:25 by pablalva          #+#    #+#             */
/*   Updated: 2026/05/03 19:13:12 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#pragma once
#include <string>
#include "Parseo_solo_toca_Pablo/Directive.hpp"
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define LIGHT_MAGENTA "\033[95m"

void log_server(const std::string host, const Directive& msg);
void log_request(const std::string& method,
                 const std::string& path,
                 const std::string& version,
                 int status);