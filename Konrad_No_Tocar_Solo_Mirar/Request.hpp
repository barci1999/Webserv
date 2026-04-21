/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 17:00:37 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/19 20:35:14 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <iostream>

class Request
{
	private:

		std::string _method;
		std::string _path; // se compara con las rutas de los bloque location 
		std::string _query;
		std::string _version;
		std::map<std::string, std::string> _headers;
		std::string _body;
		std::string _final_status;
		unsigned int _status_code;

	public:
	
		Request();
		Request(std::string req);
		Request(const Request& other);

		
		Request& operator=(const Request& other);
		~Request(){};

		const std::string get_method() const {return this->_method;}
		const std::string get_path() const {return this->_path;}
		const std::string get_query() const { return this->_query;}
		const std::string get_version() const {return this->_version;}
		const std::map<std::string,std::string>& get_headers() const {return this->_headers;}
		const std::string get_a_header(std::string to_find) const;
		const std::string get_body() const {return this->_body;}
		const std::string get_final_status() const {return this->_final_status;}
		unsigned int get_status_code() const {return this->_status_code;}

		void set_method(std::string method) {this->_method = method;}
		void set_path(std::string path) {this->_path = path;}
		void set_query(std::string query) {this->_query = query;}
		void set_version(std::string version) {this->_version = version;}
		void set_headers(std::map<std::string,std::string> headers) {this->_headers = headers;}
		void set_a_header(std::string iterator,std::string value) {this->_headers[iterator] = value;}
		void set_body(std::string body) {this->_body = body;}
		void set_final_status(std::string final_status) {this->_final_status = final_status;}
		void set_status_code(unsigned int status) {this->_status_code = status;}
};
std::ostream& operator<<(std::ostream& out,const Request& to_print);