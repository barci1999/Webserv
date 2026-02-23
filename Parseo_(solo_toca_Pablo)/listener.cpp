/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:36:03 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/23 11:24:37 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "listener.hpp"

listener::listener(std::string port)
{
	parse_input(port);
	this->_lstSocket_fd = -1;
	std::memset(&this->_lstSocketAddr,0,sizeof(this->_lstSocketAddr));
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
    if (errno == ERANGE || port < 0 || port > 65535)
        throw std::runtime_error("Port out of valid range (0-65535)");
    this->_lstPort = static_cast<int>(port);
}
listener::~listener(){}

const int listener::get_lstPort() const
{
    return this->_lstPort;
}
const int listener::get_lstSocket_fd() const
{
    return this->_lstSocket_fd;
}
const sockaddr listener::get_lstSocketAddr() const
{
    return this->_lstSocketAddr;
}

void listener::set_lstPort(int _lstPort)
{
    this->_lstPort = _lstPort;
}
void listener::set_lstSocket_fd(int _lstSocket_fd)
{
    this->_lstSocket_fd = _lstSocket_fd;
}
void listener::set_lstSocketAddr(sockaddr _lstSocketAddr)
{
    this->_lstSocketAddr = _lstSocketAddr;
}


