/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 11:08:08 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/14 16:50:58 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"  
#include "Request.hpp"    
#include "../Parseo_solo_toca_Pablo/Block.hpp" 
#include "../utils.hpp"
#include<fstream>

Response::Response(): _headers()
{
	this->_statusCode = 200;
	this->_reasonPhrase = "OK";
	this->_body = "";
	this->_version="HTTP/1.1";
}
Response::Response(std::string version,unsigned int statucode,std::string reasonphrase,std::map<std::string,std::string> headers,std::string body)
{
	this->_version = version;
	this->_statusCode = statucode;
	this->_reasonPhrase = reasonphrase;
	this->_headers = headers;
	this->_body = body;
}
Response::Response(const Response& other)
{
	this->_version = other.get_version();
	this->_statusCode = other.get_statusCode();
	this->_reasonPhrase = other.get_reasonPhrase();
	this->_headers = other.get_headers();
	this->_body = other.get_body();
}
Response::Response(const Request to_check, const server server_config)
{
    this->_version = "HTTP/1.1";
    this->_statusCode = to_check.get_status_code();
    this->_reasonPhrase = to_check.get_final_status();

    this->_headers.clear();
    this->_body.clear();

    if (this->_statusCode == 200 ||this->_statusCode == 201 ||this->_statusCode == 204)
    {
        if (to_check.get_method() == "POST")
            make_Post(to_check, server_config);
        else if (to_check.get_method() == "GET")
		{
            make_Get(to_check, server_config);
		}
        else if (to_check.get_method() == "DELETE")
		{
            make_Delete(to_check, server_config);
		}
        else
            set_error(*this, 405,server_config);
    }
	else
	{
		this->_statusCode = to_check.get_status_code();
		set_error(*this,this->_statusCode,server_config);
		
	}
}
Response& Response::operator=(const Response& other)
{
	if(this == &other)
		return *this;
	this->_version = other.get_version();
	this->_statusCode = other.get_statusCode();
	this->_reasonPhrase = other.get_reasonPhrase();
	this->_headers = other.get_headers();
	this->_body = other.get_body();
	return *this;
}
std::string Response::select_valuePhrase(unsigned int status)
{
	switch (status)
	{
	case 200:
		return("OK");
	case 201:
		return("Created");
	case 204:
		return("No Content");
	case 400:
		return("Bad Request");
	case 403:
		return("Forbidden");
	case 404:
		return("Not Found");
	case 405:
		return("Method Not Allowed");
	case 413:
		return("Payload Too Large");
	case 414:
		return("URI Too Long");
	case 500:
		return("Internal Server Error");
	case 501:
		return("Not Implemented");
	case 505:
		return("HTTP Version Not Supported");
	default:
		return("Not Implemented");
	}
}
Response::~Response(){}

std::string Response::generate_autoindex(const std::string& full_path,const std::string& request_path)
{
	DIR* dir = opendir(full_path.c_str());
	if (!dir)
	{
		return "";
	}
	std::string body;
	body += "<html><head><title>Index of " + request_path + "</title></head><body>";
    body += "<h1>Index of " + request_path + "</h1><ul>";
	dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name == "." || name == "..")
		{
			continue;
		}
		body += "<li><a href=\"" + name + "\">" + name + "</a></li>";
		
	}
	body += "</ul></body></html>";
	closedir(dir);
	return body;
}
void Response::make_Get(const Request to_check,const server server_config)
{  

	std::string path = to_check.get_path();
    if (path.empty()) {	set_error(*this,404,server_config); return; }
    Block best_location = find_best_location(path,server_config);
    size_t max_len = best_location.getName().length();
	Directive root;
	bool has_loc = false;
	if (!best_location.getName().empty())
	{
		root = search_directive("root",best_location);
		has_loc = true;
	}
	if (root.name.empty() || root.args.empty())
		root = server_config.get_srvRoot();
	std::string root_path = root.args[0];
	std::string relative = path.substr(max_len);
	std::string full_path = root_path + relative;
	std::string body;
	if (!file_exist(full_path)) 
	{
		set_error(*this,404,server_config);
		return;
	}
	if (!can_read(full_path)) { set_error(*this,403,server_config); return;}
	if (is_directory(full_path))
	{
		if(full_path[full_path.size() - 1] != '/')
			full_path += '/';
		Directive index;
		if (has_loc)
			index = search_directive("index", best_location);

		if (index.name.empty())
			index = server_config.get_srvIndex();
		if (!index.name.empty())
		{
			
			for (std::vector<std::string>::iterator it = index.args.begin(); it != index.args.end(); ++it)
			{
				std::string temp = full_path + *it;
				if (file_exist(temp) && is_file(temp) && can_read(temp))
				{
					if (!read_file(temp, body)){ set_error(*this, 500,server_config); return; }
					set_version("HTTP/1.1");
					set_statuscode(200);
					set_reasonphrase("OK");

					addback_headers("Content-Type",getContentType(temp));
					addback_headers("Content-Length",toString(body.size()));
					addback_headers("Connection","close");
					set_body(body);
					return;
				}
			}
		}
		Directive autoindex;
		if(has_loc)
			autoindex = search_directive("autoindex",best_location);
		if (autoindex.name.empty())
		{
			autoindex = server_config.get_srvAutoindex();
		}
		if (!autoindex.name.empty() && !autoindex.args.empty() && autoindex.args[0] == "on")
		{
			body = generate_autoindex(full_path,path);
			if (body.empty()){set_error(*this, 500,server_config);return;}
			set_version("HTTP/1.1");
			set_statuscode(200);
			set_reasonphrase("OK");

			addback_headers("Content-Type", "text/html");
			addback_headers("Content-Length", toString(body.size()));
			addback_headers("Connection", "close");
			set_body(body);

			return;
		}
		else
		{
			set_error(*this,403,server_config);
			return;
		}
	}
	if (!is_file(full_path)){set_error(*this,403,server_config);return;}
	if (!read_file(full_path,body)){set_error(*this,500,server_config);return;}
	set_version("HTTP/1.1");
	set_statuscode(200);
	set_reasonphrase("OK");
	addback_headers("Content-Type",getContentType(full_path));
	addback_headers("Content-Length",toString(body.size()));
	addback_headers("Connection","close");
	set_body(body);
	return;
}
void Response::set_error(Response& modifi,unsigned int error,const server& server_config)
{

    modifi.set_reasonphrase(select_valuePhrase(error));
	if (modifi.get_reasonPhrase() == "Not Implemented")
	{
		modifi.set_statuscode(501);
	}
	else
		modifi.set_statuscode(error);

	std::vector<Directive> errors = server_config.get_srvErrorPage();
	std::string error_path = "";
	std::string body;
	for (std::vector<Directive>::iterator it = errors.begin();it != errors.end(); ++it)
	{
		if (!it->args.empty() && !it->args[0].empty() && !it->args[1].empty() && it->args[0] == toString(error))
		{
			error_path = it->args[1];
		}
	}
	if (!error_path.empty() && error_path[0] == '/')
	{
		error_path.erase(0,1);	
	}
	if (error_path.empty())
	{
		modifi.set_statuscode(500);
		modifi.set_reasonphrase("Internal Server Error");
		body = "<h1>500 - Internal Server Error</h1><p>Ocurrió un error al intentar acceder al archivo.</p> <p>Por favor, inténtalo más tarde.</p>";
	}
	else
	{
		if (file_exist(error_path) && can_read(error_path))
		{
			read_file(error_path,body);
		}
		else
		{
			modifi.set_statuscode(500);
			modifi.set_reasonphrase("Internal Server Error");
			body = "<h1>500 - Internal Server Error</h1><p>Ocurrió un error al intentar acceder al archivo.</p> <p>Por favor, inténtalo más tarde.</p>";
		}
	}
    modifi.set_body(body);

    modifi.addback_headers("Content-Type", "text/html");

    std::ostringstream len;
    len << body.size();
    modifi.addback_headers("Content-Length", len.str());

    modifi.addback_headers("Connection", "close");
}



std::string Response::getContentType(const std::string& path)
{
	size_t pos = path.rfind('.');
	if (pos == std::string::npos)
	{
		return ("application/octet-stream");
	}
	std::string ext = path.substr(pos + 1);
	if (ext == "html" || ext == "htm") return "text/html";
	if (ext == "css") return "text/css";
	if (ext == "js") return "application/javascript";
	if (ext == "json") return "application/json";
	if (ext == "txt") return "text/plain";

	if (ext == "png") return "image/png";
	if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
	if (ext == "gif") return "image/gif";
	if (ext == "svg") return "image/svg+xml";
	if (ext == "ico") return "image/x-icon";

	if (ext == "pdf") return "application/pdf";

	return "application/octet-stream";
}
std::string res_to_str(const Response& to_change)
{
	std::ostringstream res;
	
	res << to_change.get_version() << " " << to_change.get_statusCode() << " " << to_change.get_reasonPhrase() << "\r\n";
	std::map<std::string, std::string> headers = to_change.get_headers();
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		res << it->first << ": " << it->second << "\r\n";
	}
	res << "\r\n";
	res << to_change.get_body();
	return res.str();
}
//si todo va bien se retorna 204

/* flujo de ejecucion
	1. resolver location (o server)
	2. comprobar método permitido → 405
	3. construir full_path
	4. ¿existe?
		→ NO → 404
	5. ¿es directorio?
		→ SÍ → 403
	6. ¿tienes permisos para borrar?
		→ NO → 403
	7. remove()
	8. responder 200 / 204
*/
void Response::make_Delete(const Request to_check,const server server_config)
{
	std::string path = to_check.get_path();
    if (path.empty()) {(*this,404,server_config);	return;	}
	
	Block best_loc = find_best_location(path,server_config);
	if (best_loc.getName().empty())	{	set_error(*this,404,server_config);	return;	}
	
	Directive root = search_directive("root",best_loc);
	if (root.name.empty() || root.args.empty())	{	set_error(*this,404,server_config);	return;	}
	
	Directive methods = search_directive("allowed_methods",best_loc);
	if (methods.name.empty() || methods.args.empty())	{	set_error(*this,405,server_config);	return;	}
	
	std::vector<std::string>::iterator it = std::find(methods.args.begin(),methods.args.end(),"DELETE");
	if (it == methods.args.end())	{	set_error(*this,405,server_config);	return;	}
	
	size_t max_len = best_loc.getName().length();
	std::string root_path = root.args[0];
	std::string relative = path.substr(max_len);
	std::string full_path = root_path + relative;
	if(!file_exist(full_path)) { set_error(*this,404,server_config);	return;	}
	if(is_directory(full_path)) {	set_error(*this,403,server_config);	return;	}
	
	std::string parent_path = take_parent_path(full_path);
	if (!can_write(parent_path))	{	set_error(*this,403,server_config);	return;	}
	if (std::remove(full_path.c_str()) != 0) {	set_error(*this,500,server_config);	return;	}
	else{
			set_version("HTTP/1.1");
			set_statuscode(204);
			set_reasonphrase(select_valuePhrase(204));
			this->_body.clear();
			this->_headers.clear();
			addback_headers("Content-Length",toString(0));
			addback_headers("Connection","close");
			return;
		}

}
// 1º sacar el path del request
	// si sale bien continuar
	// sacar error 404 si no 
// 2º find best_location con el path del request
	// si se encuentra el location continuar
	// si no sacar error 404
// 3º obtener el root del location
	// si se encuentra continuar
	// si no se encuentra usar el root del servidor
		// si no se encuentra root del servidor sacar error 404
// 4º validar que existe el metodo POST en allowed_methods del location
	// si de encuentra el metodo continuar 
	// si no se encientra sacar error 405
// 5º validar upload_enable on/off
	// si on continuar
	// si off sacar error 403
// 6º construir path real
// 7º comprobar el path
	// si existe y es un archivo continuar
	// si es un directorio sacar error 403
// 8º validar permisos de escritura dela carpeta donde esta el archivo
	// si la carpeta tiene los permisos de escritura continuar
	// si no sacar error 403
// 9º comprobar que el body existe (aun que este vacio) y tiene un tamaño permitido
	// si todo bien continuar
	// si tiene un tamaño demasiado grande sacar un 413
// 10º escribir el arhivo
	// si el archivo existe (sobreescribir el archivo) y montar un response 200 (hedares = Content-Length: 0 / Connection: close)
	// si el archivo no existe (crear el archivo y escribir en el) u montar un 201 (hedares = Content-Length: 0 / Connection: close)
	// si no se puede abrir o falla la escritura sacar un 500
void Response::make_Post(const Request to_check,const server server_config)
{
	std::string path = to_check.get_path();
	if(path.empty()) {set_error(*this,404,server_config); return;}

	Block loc = find_best_location(path,server_config);
	if(loc.getName().empty()) {set_error(*this,404,server_config); return;}

	Directive root = search_directive("root",loc);
	if(root.name.empty())
	{
		root = server_config.get_srvRoot();
		if (root.name.empty()) {set_error(*this,404,server_config); return;}
	}
	
	Directive methods = search_directive("allowed_methods",loc);
	if(methods.name.empty()) {set_error(*this,405,server_config); return;}

	std::vector<std::string>::iterator it_m = std::find(methods.args.begin(),methods.args.end(),"POST");
	if(it_m == methods.args.end()) {set_error(*this,405,server_config); return;}

	Directive upload = search_directive("upload_enable",loc);
	if(upload.name.empty()) {set_error(*this,403,server_config); return;}

	std::vector<std::string>::iterator it_up = std::find(upload.args.begin(),upload.args.end(),"on");
	if(it_up == upload.args.end()) {set_error(*this,403,server_config); return;}

	if (to_check.get_body().size() > static_cast<size_t>(server_config.get_srvClientMaxBody()))
	{
		set_error(*this,413,server_config); return;
	}
	
	size_t max_len = loc.getName().length();
	std::string root_path = root.args[0];
	std::string relative = path.substr(max_len);
	std::string full_path = root_path + relative;
	
	int fd_file = 0;
	if (file_exist(full_path))
	{
		if (is_directory(full_path)) { set_error(*this,403,server_config); return;}
		else if (is_file(full_path) && can_write(take_parent_path(full_path)))
		{
			fd_file = open(full_path.c_str(),O_WRONLY | O_TRUNC);
			set_version("HTTP/1.1");
			set_statuscode(200);
			set_reasonphrase(select_valuePhrase(200));
			this->_body.clear();
			this->_headers.clear();
			addback_headers("Content-Length",toString(0));
			addback_headers("Connection","close");
		}
		else { set_error(*this,403,server_config); return; }
	}
	else
	{
		fd_file = open(full_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,0644);
		set_version("HTTP/1.1");
		set_statuscode(201);
		set_reasonphrase(select_valuePhrase(201));
		this->_body.clear();
		this->_headers.clear();
		addback_headers("Content-Length",toString(0));
		addback_headers("Connection","close");
	}
	if (fd_file == -1)
	{
		set_error(*this,500,server_config); return;
	}
	
	size_t total_bits = 0;
	std::string body = to_check.get_body();
	while (total_bits < body.size())
	{
		ssize_t w = write(fd_file,body.c_str() + total_bits, body.size() - total_bits);
		if (w <= 0)
		{
			close(fd_file);
			set_error(*this,500,server_config);
			return;
		}
		total_bits += w;
	}
	close(fd_file);
}

