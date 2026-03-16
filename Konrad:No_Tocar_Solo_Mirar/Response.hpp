/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:51:11 by pablalva          #+#    #+#             */
/*   Updated: 2026/03/16 12:54:14 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include<map>
class Response
{
private:
	std::string _version;
	unsigned int _statusCode;
	std::string _reasonPhrase;
	std::map<std::string,std::string> _headers;
	std::string _body;
	
public:
	Response();
	Response(const Response& other);
	Response(std::string,unsigned int,std::string,std::map<std::string,std::string>,std::string);
	Response &operator=(const Response& other);
	~Response();

	const std::string get_version() const {	return this->_version ;}
	unsigned int get_statusCode() const { return this->_statusCode ;}
	const std::string get_reasonPhrase() const {return this->_reasonPhrase;}
	const std::map<std::string,std::string> get_headers() const {return this->_headers;}
	const std::string get_body() const {return this->_body;}

	void set_version(std::string version) {this->_version = version;}
	void set_statuscode(unsigned int statuscode) {this->_statusCode = statuscode;}
	void set_reasonphrase(std::string reasonphrase) {this->_reasonPhrase = reasonphrase;}
	void set_Headers(std::map<std::string,std::string> headers) {this->_headers = headers;}
	void addback_headers(std::string iterator,std::string value) {this->_headers[iterator] = value;}
	void set_body(std::string body) {this->_body = body;}
};


/*
Reason phrases comunes
Código	Reason phrase
200	OK
201	Created
204	No Content
400	Bad Request
403	Forbidden
404	Not Found
405	Method Not Allowed
413	Payload Too Large
500	Internal Server Error
501	Not Implemented
505	HTTP Version Not Supported
*/

