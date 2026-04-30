/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollLoop.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 19:02:54 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/18 21:07:18 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POLLLOOP_HPP
# define POLLLOOP_HPP
# include <vector>
# include <iostream>
# include <string>
# include <exception>
# include <sstream>
# include "./Parseo_solo_toca_Pablo/Block.hpp"
# include "./Parseo_solo_toca_Pablo/Directive.hpp"
# include "./Parseo_solo_toca_Pablo/Parser.hpp"
# include "./Parseo_solo_toca_Pablo/server.hpp"
# include "Konrad_No_Tocar_Solo_Mirar/RequestParser.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Response.hpp"
# include "client.hpp"
# include "listener.hpp"
# include "poll.h"
# include <unistd.h>
# include "Konrad_No_Tocar_Solo_Mirar/ResponseCGI.hpp"

class listener;
int pollLoop(std::vector<server> general);

#endif