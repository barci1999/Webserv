/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 15:26:58 by pablalva          #+#    #+#             */
/*   Updated: 2026/03/15 14:59:01 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "RequestParser.hpp"



Request::Request(std::string req)
{
	(void)req;
}
Request::Request()
{
	
}
Request& Request::operator=(const Request& other)
{
	if (this == &other)
	{
		return *this;
	}
	this->_method = other.get_method();
	this->_path = other.get_path();
	this->_query = other.get_query();
	this->_version = other.get_version();
	this->_headers = other.get_headers();
	this->_body = other.get_body();
	return *this;
}
std::ostream& operator<<(std::ostream& out ,const Request& other)
{
	out << "==================== Actual Request Values ====================" << std::endl;
	out << "Method -> " <<other.get_method() << std::endl;
	out << "Path -> " << other.get_path() << std::endl;
	out << "Query -> " << other.get_query() << std::endl;
	out << "Version -> "<<other.get_version() << std::endl;
	out << "Headers:" << std::endl;
	for (std::map<std::string,std::string>::const_iterator it = other.get_headers().begin() ; it != other.get_headers().end() ; it++)
	{
		out << "	header " <<it->first << " = " << it->second << std::endl;
	}
	out << "Body -> "<<other.get_body() << std::endl;
	out << "Final_status -> " << other.get_final_status() << std::endl;
	out << "Status_code -> " << other.get_status_code() << std::endl;
	return out;
}