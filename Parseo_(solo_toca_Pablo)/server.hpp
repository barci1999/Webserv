/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 19:02:54 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/17 18:32:28 by pablalva         ###   ########.fr       */
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

class server
{
private:
	
	Directive _srvPorts;
	std::string _srvName;
	Directive _srvRoot;
	Directive _srvIndex;
	Directive _srvErrorPage;
	std::list<Block> _srvLocations;
	// atributos listen para los puertos
	// gestion del nombre del servidor
	// root de inicio del propio server 
	// gestion del archivo defaut del servidor cuando arracnque
	// gestion de paginas de error por default
	// gestion de tamaño maximo de las peticiones de los clientes 
	// gestion de autoindex
	/*bloques location {
		- comprobacion de si autoindex on o off
		- gestion de index para archivos por default
		- gestion de root para inicio de ruta en el location
		(en el caso de peticiones de post se debe gestiona el tamaño maximo de archivo)
		- gestion de formato para los cgi
	}*/
	bool server::check_location_block(Block to_check);
	bool check_locations(const std::list<Block>);
	Directive check_directives(std::string to_serch, const Block& to_check);

public:
	server(const Block& block);
	~server();
};




#endif