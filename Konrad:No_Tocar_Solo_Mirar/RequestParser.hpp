/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 17:28:06 by ksudyn            #+#    #+#             */
/*   Updated: 2026/02/23 19:55:07 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include <sstream>

class RequestParser
{
    private:
        void ParseRequestLine(const std::string& line, Request& request);
        void ParseHeaders(const std::string& headersText, Request& request);
        void ParseBody(const std::string& bodyContent, Request& request);

        std::string Trim(const std::string& str);
    public:
        Request parse(const std::string& rawRequest);
    
};
