/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:23:37 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/30 16:03:57 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENER_HPP
# define LISTENER_HPP
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <cstring>
# include <exception>
# include <iostream>
# include "Parseo_solo_toca_Pablo/server.hpp"
# include <unistd.h>

class listener
{
private:
	int _lstPort;
	int _lstSocket_fd;
	sockaddr_in _lstSocketAddr;
	server *originalsrv;
	
	void parse_input(const std::string& input);
	void init_lstSocketAddr();
public:
	listener(std::string port,server& conf);
	listener(const listener& to_copy);
	~listener();

	int get_lstPort() const;
	int get_lstSocket_fd() const;
	const sockaddr_in get_lstSocketAddr() const;
	server &get_originalsrv() const;

	void set_lstPort(int _lstPort);
	void set_lstSocket_fd(int _lstSocket_fd);
	void set_lstSocketAddr(sockaddr_in _lstSocketAddr);
	void set_originalsrv(server *);
};

#endif