/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 17:51:11 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/20 14:26:42 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include<map>
#include"Request.hpp"
#include"../Parseo_solo_toca_Pablo/server.hpp"
#include"../Parseo_solo_toca_Pablo/Block.hpp"
#include<sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "../utils.hpp"
#include<algorithm>
#include <iostream>
#include <string>
//cosas a comprobar a la hora de hacer la response:
// en el parse recuest no se saca error en el query si se envuentra && asi que
//se debe hacer un parseo si se encuentran esos caractters antes de lanzar la response 
class Response
{
private:
	std::string _version;
	unsigned int _statusCode;
	std::string _reasonPhrase;
	std::map<std::string,std::string> _headers;
	std::string _body;
	

	void set_error(Response& modifi,unsigned int error,const server& server_config);
public:
	Response();
	Response(const Request to_check,const server server_config);
	Response(const Response& other);
	Response(std::string,unsigned int,std::string,std::map<std::string,std::string>,std::string);
	Response &operator=(const Response& other);
	~Response();

	std::string getContentType(const std::string& to_check);
	std::string generate_autoindex(const std::string& ful_path,const std::string& request_path);
	std::string extract_body(const std::string& raw_body, const std::string& boundry);
	std::string extract_boundry(const std::string& raw_boundry);
	std::string extract_filename(const std::string& raw_body);
	
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

	void make_Post(const Request,const server server_config);
	void make_Get(const Request,const server server_config);
	void make_Delete(const Request,const server server_config);
	std::string select_valuePhrase(unsigned int);
};
std::string res_to_str(const Response& to_change);


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
414 Uri Too Long
500	Internal Server Error
501	Not Implemented
505	HTTP Version Not Supported
*/


/*buscar informacion sobre el 413*/
