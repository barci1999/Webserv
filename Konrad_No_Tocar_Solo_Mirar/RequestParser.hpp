/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 17:28:06 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/14 14:54:05 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include "../utils.hpp"
#include <sstream>


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
		static tokens is_chunked(const Request& to_check);
		static tokens has_cont_length(const Request& to_check);
		static std::string normalize_path(const std::string& path);
	public:
		static Request& parse(const std::string& rawRequest,Request &request);
		static bool valid_request(Request& to_check);
};