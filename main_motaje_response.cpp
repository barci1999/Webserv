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
    std::string raw_request = "GET / HTTP/1.1\r\nContent-Length: 0\r\nHost: example.com\r\n\r\n";
    Request req;
	RequestParser::parse(raw_request,req);
	RequestParser::valid_request(req);
	std::cout << req<<std::endl;

    // 🔹 2. Crear configuración del servidor (simulada)
    Block server_root;
	Block server;

    // root = carpeta donde están tus archivos (ajusta si hace falta)
    Directive root;
    root.name = "root";
    root.args.push_back("Parseo_solo_toca_Pablo/www");

    // index = archivo por defecto
    // Directive index;
    // index.name = "index";
    // index.args.push_back("index.html");

    // autoindex off (opcional)
    Directive autoindex;
    autoindex.name = "autoindex";
    autoindex.args.push_back("on");

	server.setName("/");
    server.addDirective(root);
    //server.addDirective(index);
    server.addDirective(autoindex);
	server_root.addChild(server);


	server_root.print();

	std::cout << "======================================================" <<std::endl;

    // 🔹 3. Crear response
	req.set_status_code(404);
    Response res(req,server_root);

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

	std::cout << "=========================================================" <<std::endl;
	std::cout << res_to_str(res) << std::endl;

    return 0;
}