/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 17:28:06 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/10 21:39:18 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include <sstream>

enum e_check
{
    OK,
    KO,
    ERROR
};

class RequestParser
{
	private:
		static void ParseRequestLine(const std::string& line, Request& request);
		static void ParseHeaders(const std::string& headersText, Request& request);
		static void ParseBody(const std::string& bodyContent, Request& request);

		static bool valid_method(Request& to_check);
		static bool valid_path( Request& to_check);
		static bool valid_query( Request& to_check);
		static bool valid_version( Request& to_check);
		static bool valid_headers( Request& to_check);
		static bool valid_body( Request& to_check);
		static void set_error(Request& req, unsigned int code,const std::string error_phrase);
		static std::string Trim(const std::string& str);
		static e_check is_chunked(const Request& to_check);
		static e_check has_cont_length(const Request& to_check);
	public:
		static Request& parse(const std::string& rawRequest,Request &request);
		static bool valid_request(Request& to_check);
};