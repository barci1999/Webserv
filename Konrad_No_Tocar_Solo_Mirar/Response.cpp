/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 11:08:08 by pablalva          #+#    #+#             */
/*   Updated: 2026/03/30 18:59:00 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"  
#include "Request.hpp"    
#include "../Parseo_solo_toca_Pablo/Block.hpp" 

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
Response::Response(const Request to_check,const Block server_config)
{
	this->_version = to_check.get_version();
	this->_statusCode = to_check.get_status_code();
	this->_reasonPhrase = to_check.get_final_status();
	this->_headers = to_check.get_headers();
	this->_body = to_check.get_body();
	if (to_check.get_status_code() == 200)
	{
		if (to_check.get_method() == "POST")
		{
			make_Post(to_check,server_config);
		}
		else if (to_check.get_method() == "GET")
		{
			make_Get(to_check,server_config);
			std::cout<<this->get_statusCode()<<std::endl;
		}
		else if (to_check.get_method() == "DELETE")
		{
			make_Delete(to_check,server_config);
		}
		else
		{
			set_error(*this,405);
		}
	}
	else
	{
		this->_statusCode = to_check.get_status_code();
		this->_reasonPhrase = select_valuePhrase(to_check.get_status_code());
		
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
Directive Response::search_directive(std::string to_cheack,const Block block)
{
	Directive result;
	result.name = "";
	const std::vector<Directive>& directive = block.getDirectives();
	std::vector<Directive>::const_iterator it = directive.begin();
	for (; it != directive.end(); it++)
	{
		if (to_cheack == it->name)
		{
			result.name = it->name;
			result.args = it->args;
			return result;
		}
	}
	return result;
}
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
static std::string toString(unsigned int value) 
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
void Response::make_Get(const Request to_check,const Block server_config)
{  

	std::string path = to_check.get_path();
    if (path.empty()) {	set_error(*this,404); return; }
    Block best_location;
    size_t max_len = 0;
    for (std::list<Block>::const_iterator it = server_config.getBlocks().begin();it != server_config.getBlocks().end(); ++it)
    {
		
        std::string loc = it->getName();

        if (path.find(loc) == 0 && loc.length() > max_len)
        {
            best_location = *it;
            max_len = loc.length();
        }
    }
	
	if(max_len == 0){ set_error(*this,404); return; }
	Directive root = search_directive("root",best_location);
	if (root.name.empty() || root.args.empty())
		root = search_directive("root",server_config);
	std::string root_path = root.args[0];
	std::string relative = path.substr(max_len);
	if (root_path[root_path.size() -1] != '/')
	{
		root_path += '/';
	}
	std::string full_path = root_path + relative;
	std::string body;
	if (!file_exist(full_path)) 
	{
		set_error(*this,404);
		std::cout<<this->get_statusCode()<<std::endl;
		return;
	}
	if (!can_read(full_path)) { set_error(*this,403); return;}
	if (is_directory(full_path))
	{
		if(full_path[full_path.size() - 1] != '/')
			full_path += '/';
		Directive index = search_directive("index",best_location);
		if (!index.name.empty())
		{
			
			for (std::vector<std::string>::iterator it = index.args.begin(); it != index.args.end(); ++it)
			{
				std::string temp = full_path + *it;
				if (file_exist(temp) && is_file(temp) && can_read(temp))
				{
					if (!read_file(temp, body)){ set_error(*this, 500); return; }
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
		Directive autoindex = search_directive("autoindex",best_location);
		if (!autoindex.name.empty() && !autoindex.args.empty() && autoindex.args[0] == "on")
		{
			body = generate_autoindex(full_path,path);
			if (body.empty()){set_error(*this, 500);return;}
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
			std::cout<<"HOLAAAAAAAAAAAAAAAAAA"<<std::endl;
			set_error(*this,403);
			return;
		}
	}
	if (!is_file(full_path)){set_error(*this,403);return;}
	if (!read_file(full_path,body)){set_error(*this,500);return;}
	set_version("HTTP/1.1");
	set_statuscode(200);
	set_reasonphrase("OK");
	addback_headers("Content-Type",getContentType(full_path));
	addback_headers("Content-Length",toString(body.size()));
	addback_headers("Connection","close");
	set_body(body);
	return;
}
void Response::set_error(Response& modifi,unsigned int error)
{
	modifi.set_statuscode(error);
	modifi.set_reasonphrase(select_valuePhrase(error));
}

bool Response::is_directory(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        return false; // error, no existe o no accesible
    return S_ISDIR(info.st_mode);
}
bool Response::file_exist(const std::string file)
{
	struct stat buffer;
	return (stat(file.c_str(),&buffer) == 0);
}
bool Response::is_file(const std::string file)
{
	struct stat buffer;
	if (stat(file.c_str(),&buffer) != 0)
	{
		return false;
	}
	return S_ISREG(buffer.st_mode);
}
bool Response::can_read(const std::string file)
{
	return (access(file.c_str(),R_OK) == 0);
}
bool Response::read_file(const std::string& path, std::string& out)
{
    std::ifstream file(path.c_str());
    if (!file.is_open())
        return false;

    std::stringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();
    return true;
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
void Response::make_Delete(const Request to_check,const Block server_config)
{

}
void Response::make_Post(const Request to_check,const Block server_config)
{

}

