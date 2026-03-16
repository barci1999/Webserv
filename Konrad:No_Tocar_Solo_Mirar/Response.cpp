/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 11:08:08 by pablalva          #+#    #+#             */
/*   Updated: 2026/03/16 12:54:18 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"Response.hpp"

Response::Response(): _headers()
{
	this->_statusCode = 200;
	this->_reasonPhrase = "OK";
	this->_body = "";
	this->_version="HTTP/1.1";
}
Response::Response(std::string version,unsigned int statucode,std::string reasonphrase,std::map<std::string,std::string> headers,std::string body)
{
	this->_version = version;
	this->_statusCode = statucode;
	this->_reasonPhrase = reasonphrase;
	this->_headers = headers;
	this->_body = body;
}
Response::Response(const Response& other)
{
	this->_version = other.get_version();
	this->_statusCode = other.get_statusCode();
	this->_reasonPhrase = other.get_reasonPhrase();
	this->_headers = other.get_headers();
	this->_body = other.get_body();
}
Response& Response::operator=(const Response& other)
{
	if(this == &other)
		return *this;
	this->_version = other.get_version();
	this->_statusCode = other.get_statusCode();
	this->_reasonPhrase = other.get_reasonPhrase();
	this->_headers = other.get_headers();
	this->_body = other.get_body();
	return *this;
}

