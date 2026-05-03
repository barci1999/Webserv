/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:36:03 by pablalva          #+#    #+#             */
/*   Updated: 2026/05/03 18:17:34 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "listener.hpp"
#include <cerrno>
#include <stdlib.h>


listener::listener(std::string port,server& conf)
{
	int opt = 1;


	this->originalsrv = &conf;
	parse_input(port);
	this->_lstSocket_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(this->_lstSocket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	init_lstSocketAddr();
	if(bind(this->_lstSocket_fd,(struct sockaddr*) &this->_lstSocketAddr, sizeof(this->_lstSocketAddr)) == -1)
	{
		close(this->_lstSocket_fd);
		throw std::invalid_argument("Failed bind");
		
	}
	listen(this->_lstSocket_fd,3);
}

listener::listener(const listener& to_copy)
{
	this->_lstPort=to_copy._lstPort;
	this->_lstSocket_fd=to_copy._lstSocket_fd;
	this->_lstSocketAddr=to_copy._lstSocketAddr;
	this->originalsrv=to_copy.originalsrv;
}

void listener::parse_input(const std::string& input)
{
	if (input.empty())
		throw std::runtime_error("Port can't be empty");
	char* endptr = NULL;
	errno = 0;
	long port = std::strtol(input.c_str(), &endptr, 10);
	if (endptr == input.c_str())
		throw std::runtime_error("Port must be a numeric value");
	if (*endptr != '\0')
		throw std::runtime_error("Port must contain only digits");
	if (errno == ERANGE || port < 1024 || port > 65535)
		throw std::runtime_error("Port out of valid range (1024-65535)");
	this->_lstPort = static_cast<int>(port);
}
listener::~listener(){}

int listener::get_lstPort() const
{
	return this->_lstPort;
}
int listener::get_lstSocket_fd() const
{
	return this->_lstSocket_fd;
}
const sockaddr_in listener::get_lstSocketAddr() const
{
	return this->_lstSocketAddr;
}

server &listener::get_originalsrv() const
{
	return *originalsrv;
}

void listener::set_lstPort(int _lstPort)
{
	this->_lstPort = _lstPort;
}
void listener::set_lstSocket_fd(int _lstSocket_fd)
{
	this->_lstSocket_fd = _lstSocket_fd;
}
void listener::set_lstSocketAddr(sockaddr_in _lstSocketAddr)
{
	this->_lstSocketAddr = _lstSocketAddr;
}

void listener::set_originalsrv(server *originalsrv)
{
	this->originalsrv = originalsrv;
}

void listener::init_lstSocketAddr(void)
{
	this->_lstSocketAddr.sin_family = AF_INET;
	this->_lstSocketAddr.sin_port = htons(this->_lstPort);
	this->_lstSocketAddr.sin_addr.s_addr = inet_addr(this->originalsrv->get_srvHost().args[0].c_str());
}


