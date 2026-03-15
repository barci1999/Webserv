#include "Request.hpp"
#include "RequestParser.hpp"
#include "iostream"


int main(void)
{
	std::string test = "GET /submit-form HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 27\r\n\r\nname=Juan&age=25&city=Madrid";
	//std::cout << test<< std::endl;
	Request hola;
	hola = RequestParser::parse(test);
	std::cout << hola.get_method() <<std::endl;
	std::cout << hola << std::endl;
}