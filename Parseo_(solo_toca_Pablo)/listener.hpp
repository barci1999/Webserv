/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:23:37 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/23 11:10:15 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENER_HPP
# define LISTENER_HPP
# include <string>
# include <sys/socket.h>
# include  <cstring>
# include <exception>
# include <iostream>
class listener
{
private:
	int _lstPort;
	int _lstSocket_fd;
	sockaddr _lstSocketAddr;
	
	void parse_input(const std::string& input);
	
public:
	listener(std::string port);
	~listener();

	const int get_lstPort() const;
	const int get_lstSocket_fd() const;
	const sockaddr get_lstSocketAddr() const;

	void set_lstPort(int _lstPort);
	void set_lstSocket_fd(int _lstSocket_fd);
	void set_lstSocketAddr(sockaddr _lstSocketAddr);
	
};

#endif