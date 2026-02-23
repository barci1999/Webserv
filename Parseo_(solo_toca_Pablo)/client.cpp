/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 14:02:23 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/23 11:40:05 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "client.hpp"

client::client()
{
	this->ptr = NULL;
	this->fd = -1;
}
client::~client(){}

const listener* client::get_ptr() const
{
	return this->ptr;
}
const int client::get_fd() const
{
	return this->fd;
}
void client::set_ptr(listener* ptr)
{
	this->ptr = ptr;
}
void client::set_fd(int fd)
{
	this->fd = fd;
}