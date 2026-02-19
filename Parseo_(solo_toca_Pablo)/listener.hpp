/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:23:37 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/19 11:19:23 by pablalva         ###   ########.fr       */
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
	
};

#endif