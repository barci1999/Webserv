#include "Request.hpp"
#include "RequestParser.hpp"
#include "iostream"


int main(void)
{
	std::string test = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
	//std::cout << test<< std::endl;
	Request hola;
	hola = RequestParser::parse(test, hola);
	std::cout << hola.get_status_code() <<std::endl;
	RequestParser::valid_request(hola);
	std::cout << hola <<  std::endl;
}