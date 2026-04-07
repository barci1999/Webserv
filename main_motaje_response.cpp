#include <iostream>
#include <fstream>
#include "Parseo_solo_toca_Pablo/Directive.hpp"
#include "Parseo_solo_toca_Pablo/Block.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Request.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Response.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/RequestParser.hpp"
#include <stdlib.h>

int main()
{
    // 🔹 1. Crear request desde string
    std::string raw_request = 
    "GET /test/ HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Connection: close\r\n"
    "\r\n";
    Request req;
	RequestParser::parse(raw_request,req);
	RequestParser::valid_request(req);
	//std::cout << req<<std::endl;
	
	Block location;
	server Server;
	std::string srv_name = "name";
	Directive srv_ports;
	srv_ports.name = "listen";
	srv_ports.args.push_back("8080");
	Directive srv_root;
	srv_root.name = "root";
	srv_root.args.push_back("Parseo_solo_toca_Pablo/www/sandbox");
	// Directive srv_index;
	// srv_index.name = "index";
	// srv_index.args.push_back("parrot.html");
	Directive srv_Error;
	srv_Error.name = "error_page";
	srv_Error.args.push_back("404");
	srv_Error.args.push_back("Parseo_solo_toca_Pablo/www/error_pages/404.html");
	Directive srv_autoindex;
	srv_autoindex.name = "autoindex";
	srv_autoindex.args.push_back("on");

	location.setName("/test");
	Directive root_locattion;
	root_locattion.name = "root";
	root_locattion.args.push_back("Parseo_solo_toca_Pablo/www/sandbox");
	// Directive index_location;
	// index_location.name = "index";
	// index_location.args.push_back("parrot.html");
	// Directive autoindex_location ;
	// autoindex_location.name = "autoindex";
	// autoindex_location.args.push_back("on");
	Directive methods;
	methods.name = "allowed_methods";
	methods.args.push_back("GET");
	location.addDirective(methods);
	location.addDirective(root_locattion);
	//location.addDirective(index_location);
	//location.addDirective(autoindex_location);
	
	//location.print();

	std::list<Block> to_isert;
	to_isert.push_back(location);

	Server.set_srvName(srv_name);
	Server.set_srvPorts(srv_ports);
	Server.set_srvRoot(srv_root);
	//Server.set_srvIndex(srv_index);
	Server.set_srvErrorPage(srv_Error);
	Server.set_srvClientMaxBody(10485760);
	Server.set_srvAutoindex(srv_autoindex);
	Server.set_srvLocations(to_isert);

	//std::cout<<Server<<std::endl;

	
	//std::cout << "======================================================" <<std::endl;
	
	Response hola(req,Server);
	std::cout << res_to_str(hola) << std::endl;
	std::cout<<req<<std::endl;

    // 🔹 4. Ejecutar GET


    // 🔹 5. Imprimir resultado
    // std::cout << "HTTP version: " << res.get_version() << std::endl;
    // std::cout << "Status code: " << res.get_statusCode() << std::endl;
    // std::cout << "Reason phrase: " << res.get_reasonPhrase() << std::endl;

    // std::cout << "Headers:" << std::endl;
    // std::map<std::string, std::string> headers = res.get_headers();
    // for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    // {
    //     std::cout << it->first << ": " << it->second << std::endl;
    // }

    // std::cout << "Body:" << std::endl;
    // std::cout << res.get_body() << std::endl;

	//std::cout << "=========================================================" <<std::endl;
	//std::cout << res_to_str(res) << std::endl;

    return 0;
}