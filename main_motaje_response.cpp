#include <iostream>
#include <fstream>
#include "Parseo_solo_toca_Pablo/Directive.hpp"
#include "Parseo_solo_toca_Pablo/Block.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Request.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Response.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/RequestParser.hpp"
#include "Parseo_solo_toca_Pablo/Parser.hpp"
#include <stdlib.h>

int main()
{

	// 🔹 1. Crear request desde string
	try
	{
	/* code */
	std::string raw_request =
"GET /index.html HTTP/1.1\r\n"
"Host: localhost\r\n"
"Transfer-Encoding: chunked\r\n"
"\r\n";
	Request req;
	RequestParser::parse(raw_request,req);
	std::cout<<"=============================================================="<<std::endl;
	std::cout << req.get_body()<<std::endl;
	RequestParser::valid_request(req);
	std::cout<<"=============================================================="<<std::endl;
	std::cout << req.get_body()<<std::endl;
	std::cout << req<<std::endl;
	Parser parser;
	std::vector<server> list = parser.parseFile("conf_test_response.txt");
	std::vector<server>::iterator it = list.begin();
	server Server = *it;

	// std::cout<<Server<<std::endl;
	// std::cout << "======================================================" <<std::endl;
	
	// Response hola(req,Server);
	// std::cout << res_to_str(hola) << std::endl;

	// std::cout << "======================================================" <<std::endl;
	
	}
	catch(const std::exception& e)
	{
	std::cerr << e.what() << '\n';
	}


    return 0;
}