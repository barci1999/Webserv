/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 10:22:31 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/12 20:04:54 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "listener.hpp"
class client
{
private:
	listener *ptr;
	int fd;
	std::string request;
public:
	client();
	client(int fd);
	~client();
	
	const listener* get_ptr() const;
	const int get_fd() const;

	void set_ptr(listener *ptr);
	void set_fd(int fd);
};
#endif