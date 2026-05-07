/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseCGI.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 15:21:24 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/07 18:43:41 by pablalva         ###   ########.fr       */
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

void CGI_Response(Request& request, server& serv, std::vector<pollfd>& pollFds,
    std::map<int, CGIProcess*>& cgi_map, std::map<int, int>& cgi_to_client, int client_fd)
{
    CGIProcess* cgi = new CGIProcess();

    if (!cgi->initCGI(request, serv))
    {
        delete cgi;
        return;
    }
    
    // 1. execute ya añade los FDs al vector pollFds (según vimos antes)
    cgi->execute(request, serv, pollFds);

    // 2. Registrar el pipe de LECTURA (Salida del CGI -> Entrada del Server)
    int out_fd = cgi->getFD(); 
    cgi_map[out_fd] = cgi;
    cgi_to_client[out_fd] = client_fd;

    // 3. Registrar el pipe de ESCRITURA (Solo si es POST)
    int in_fd = cgi->getInputFd();
	if (out_fd != -1) {
        cgi_map[out_fd] = cgi;
        cgi_to_client[out_fd] = client_fd;
        std::cout << "MAP: Guardado out_fd " << out_fd << std::endl;
    }
    if (in_fd != -1) {
        cgi_map[in_fd] = cgi;
        cgi_to_client[in_fd] = client_fd;
        std::cout << "MAP: Guardado in_fd " << in_fd << std::endl;
    }
}
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

// Esta función:

// Crea el CGI
// Hace fork + execve
// Obtiene el pipe de salida
// Lo mete en poll()
// Guarda quién es quién

// Y TERMINA

// NO hace poll()
// NO espera resultado
// NO devuelve Response
//Solo dice: “oye pollLoop, vigila este fd”


//FUNCION QUE NO VA AQUI.
//AQUI VERIFICO SI ES CGI Y SI LO ES, EJECUTO LO DEL CGI CON POLL Y SI NO RETORNO EL Response( req, srv);
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
	//comprobar existencia del path del archivo
	std::string full_path = Response::build_full_path(path,loc,server);
	if (access(full_path.c_str(),F_OK) != 0){modyfy.set_error(modyfy,404,server); return false;}
	if(access(full_path.c_str(),R_OK)!= 0 ){modyfy.set_error(modyfy,403,server); return false;}
	return true;
	
	
	
	
}
bool handleRequest(Request& request,server& serv,std::vector<pollfd>& pollFds,
    std::map<int, CGIProcess*>& cgi_map, std::map<int, int>& cgi_to_client, int client_fd, Response& response)
{
	std::cout<<request<<std::endl;
    if (isCGI(request))
    {
		if(!check_conf_cgi(request,serv,response))
		{
			return true;
		}
        CGI_Response(request, serv, pollFds, cgi_map, cgi_to_client, client_fd);
        return false; // aqui sún no hay respuesta
    }//Un CGI NO devuelve respuesta…, devuelve un evento futuro
	else
		response = Response(request, serv);

    return true;
}
