/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 19:02:54 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/18 21:07:18 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "Directive.hpp"
# include <vector>
# include <iostream>
# include <string>
# include "Block.hpp"
# include <exception>
# include <sstream>

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


	//una vector de clases  (listeners) que tenga dentro int socket_fd / struct socket_addre / un solo puerto de la variable _srvPorts 
			// el costructor de esta clase tiene que recibir el puerto y el resto se inicializa a 0;
	//std::map de clases (client) que estas tienen que tener : int fd(iniciar como -1) , puntero al tipo de clase del array antrerior ,


	// crear dos clases nuevas para rodrigo con los vlores de arriba
	
	
	
	/*bloques location {
		- comprobacion de si autoindex on o off
		- gestion de index para archivos por default
		- gestion de root para inicio de ruta en el location
		(en el caso de peticiones de post se debe gestiona el tamaño maximo de archivo)
		- gestion de formato para los cgi
	}*/
	bool check_location_block(Block to_check);
	bool check_locations(const std::list<Block>);
	Directive check_directives(std::string to_serch, const Block& to_check);
	bool check_autoindex(const Block& to_check);
	size_t check_client_max_body(const Block& to_check);
public:
	server(const Block& block);
	~server();
};

#endif