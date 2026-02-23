/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 19:02:54 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/23 13:51:44 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "Directive.hpp"
# include "Parser.hpp"
# include <vector>
# include <iostream>
# include <string>
# include "Block.hpp"
# include <exception>
# include <sstream>
# include "client.hpp"
# include "listener.hpp"
# include <map>

class server
{
private:
//valores para konrad:

	std::string _srvName;
	Directive _srvPorts;
	Directive _srvRoot;
	Directive _srvIndex;
	Directive _srvErrorPage;
	std::list<Block> _srvLocations;
	size_t _srvClientMaxBody;
	bool _srvAutoIndex;

	
//valores para rodri:

	std::vector<listener> _srvListeners;
	std::map<int,client> _srvClients; 

	
	bool check_location_block(const Block& to_check);
	bool check_locations(const std::list<Block>);
	Directive check_directives(std::string to_serch, const Block& to_check);
	bool check_autoindex(const Block& to_check);
	size_t check_client_max_body(const Block& to_check);


public:
	server(const Block& block);
	~server();

	const	std::string get_srvName()	const;
	const	Directive get_srvPorts()	const;
	const	Directive get_srvRoot()		const;
	const	Directive get_srvIndex()	const;
	const	Directive get_srvErrorPage()	const;
	const	std::list<Block> get_srvLocations() const;
	const	size_t get_srvClientMaxBody()	const;
	const	bool get_srvAutoIndex()		const;
	const	std::vector<listener> get_srvListeners()	const;
	const	std::map<int,client> get_srvClients()	const;
};

#endif