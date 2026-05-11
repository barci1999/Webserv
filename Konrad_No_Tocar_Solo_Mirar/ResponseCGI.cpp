/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 15:21:24 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/11 19:33:58 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ResponseCGI.hpp"
#include <map>
#include <iostream>
#include <unistd.h>
#include <poll.h>

#include "ResponseCGI.hpp"
#include <map>
#include <iostream>
#include <unistd.h>
#include <poll.h>

/**
 * CGI_Response
 * ------------
 * Esta función es la "lanzadora". Su trabajo no es generar una respuesta, 
 * sino crear el proceso externo y registrarlo en nuestro sistema de vigilancia (poll).
 * * 1. Crea una nueva instancia de proceso CGI.
 * 2. Ejecuta el fork/execve (sin bloquear el servidor).
 * 3. Registra los "tuberías" (pipes) en mapas para que, cuando el poll nos avise,
 * sepamos qué CGI está escribiendo y a qué cliente pertenece ese resultado.
 */
void CGI_Response(Request& request, server& serv, std::vector<pollfd>& pollFds,
    std::map<int, CGIProcess*>& cgi_map, std::map<int, int>& cgi_to_client, int client_fd)
{
    CGIProcess* cgi = new CGIProcess();

    if (!cgi->initCGI(request, serv))
    {
        delete cgi;
        return;
    }
    
    // Inicia el proceso hijo y configura pipes asíncronos
    cgi->execute(request, serv, pollFds);

    // Registramos el pipe de salida (donde el CGI escribe su respuesta)
    int out_fd = cgi->getFD(); 
    if (out_fd != -1) {
        cgi_map[out_fd] = cgi;
        cgi_to_client[out_fd] = client_fd;
    }

    // Si es un POST, registramos también el pipe de entrada para enviarle el cuerpo (body)
    int in_fd = cgi->getInputFd();
    if (in_fd != -1) {
        cgi_map[in_fd] = cgi;
        cgi_to_client[in_fd] = client_fd;
    }
}

/**
 * isCGI
 * -----
 * Una comprobación rápida: ¿El archivo termina en .py o .php? 
 * Es el primer filtro antes de entrar en lógica compleja de configuración.
 */
bool isCGI(const Request& to_check)
{
	std::string path = to_check.get_path();
	size_t dot = path.rfind('.');
	if (dot == std::string::npos)
	{
		return false;
	}
	std::string extension = path.substr(dot);
	if (extension == ".py" || extension == ".php")
	{
		return true;
	}
	return false;
	
}

/**
 * check_conf_cgi
 * --------------
 * El "Validador de Seguridad y Reglas". Antes de lanzar un CGI, verificamos:
 * 1. ¿La ubicación (location) existe?
 * 2. ¿El método (GET/POST) está permitido aquí?
 * 3. ¿Las extensiones y los ejecutables están bien configurados en el .conf?
 * 4. ¿El archivo existe realmente en el disco y tenemos permisos?
 * * Si algo falla, preparamos la respuesta de error (404, 405, 500) de inmediato.
 */
bool check_conf_cgi(Request& req,server& server,Response& modyfy)
{
	modyfy = Response(req,server);
	std::string path = req.get_path();
	Block loc =find_best_location(req.get_path(),server);
	
	if (loc.getName().empty())
	{
		modyfy.set_error(modyfy,404,server);
		return false;
	}
	// Verificación de métodos permitidos
	Directive methods = search_directive("allowed_methods",loc);
	if (methods.name.empty() && req.get_method() != "GET")
	{
		modyfy.set_error(modyfy,405,server);
		return false;
	}
	{
		std::vector<std::string>::iterator poss = find(methods.args.begin(),methods.args.end(),req.get_method());
		if (poss == methods.args.end() && req.get_method() != "GET")
		{
			modyfy.set_error(modyfy,405,server);
			return false;
		}
	}

	// Verificación de extensiones y ejecutables configurados
	Directive ext = search_directive("cgi_extension",loc);
	Directive pass = search_directive("cgi_pass",loc);
	std::string ext_file = req.get_path().substr(req.get_path().rfind('.'));
	
	if (ext.name.empty() || pass.name.empty()){	modyfy.set_error(modyfy,500,server); return false;}
	{
		std::vector<std::string>::iterator poss = find(ext.args.begin(),ext.args.end(),ext_file);
		if (poss == ext.args.end()){
			modyfy.set_error(modyfy,500,server);return false;
		}
	}
	{
		if (ext_file == ".py")
		{
			std::vector<std::string>::iterator poss = find(pass.args.begin(),pass.args.end(),"usr/bin/python3");
			if(poss == pass.args.end()){
				modyfy.set_error(modyfy,500,server);return false;
			}
		}
		else if(ext_file == ".php")
		{
			std::vector<std::string>::iterator poss = find(pass.args.begin(),pass.args.end(),"usr/bin/php");
			if(poss == pass.args.end()){
				modyfy.set_error(modyfy,500,server);return false;
			}
		}
		else
			{modyfy.set_error(modyfy,500,server); return false;}
	}
	// Seguridad: comprobamos existencia y permisos del archivo en el sistema
	std::string full_path = Response::build_full_path(path,loc,server);
	if (access(full_path.c_str(),F_OK) != 0){modyfy.set_error(modyfy,404,server); return false;}
	if(access(full_path.c_str(),R_OK)!= 0 ){modyfy.set_error(modyfy,403,server); return false;}
	return true;
	
	
	
	
}

/**
 * handleRequest
 * -------------
 * El "Director de Tráfico" principal.
 * * - Si es un archivo normal: Genera la respuesta y devuelve 'true' (está lista para enviar).
 * - Si es un CGI: Lanza el proceso y devuelve 'false'. 
 * * ¿Por qué devuelve 'false'? Porque en HTTP asíncrono, la respuesta del CGI 
 * NO existe todavía. La respuesta nacerá más tarde, cuando el poll() nos diga 
 * que el script ha terminado de escribir en el pipe.
 */
bool handleRequest(Request& request,server& serv,std::vector<pollfd>& pollFds,
    std::map<int, CGIProcess*>& cgi_map, std::map<int, int>& cgi_to_client, int client_fd, Response& response)
{
    if (isCGI(request))
    {
		// Primero validamos la configuración
		if(!check_conf_cgi(request,serv,response))
		{
			// Si hay error en conf (ej. 404), la respuesta de error ya está lista
			return true;
		}
		// Si todo ok, lanzamos el proceso asíncrono
        CGI_Response(request, serv, pollFds, cgi_map, cgi_to_client, client_fd);
        return false; // Indica que la respuesta no está lista todavía
    }
	else
	{
		// Caso normal: archivos estáticos
		response = Response(request, serv);
	}

    return true;
}
