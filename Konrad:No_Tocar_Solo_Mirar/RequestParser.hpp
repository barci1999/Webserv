/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 17:28:06 by ksudyn            #+#    #+#             */
/*   Updated: 2026/02/19 21:26:25 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include <sstream>

class RequestParser
{
    private:
        void parseRequestLine(const std::string& line, Request& request);
        void parseHeaders(const std::string& headersText, Request& request);
        void parseBody(const std::string& bodyContent, Request& request);

        std::string trim(const std::string& str);
    public:
        Request parse(const std::string& rawRequest);
    
};
