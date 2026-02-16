/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 19:02:54 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/16 19:28:08 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

class server
{
private:
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

	
public:
	server(/* args */);
	~server();
};

server::server(/* args */)
{
}

server::~server()
{
}


#endif