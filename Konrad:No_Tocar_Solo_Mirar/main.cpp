#include "Request.hpp"
#include "RequestParser.hpp"
#include "iostream"
#include <vector>


int main(void)
{

    std::vector<std::string> tests;

    tests.push_back("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
    tests.push_back("GET /search?q=juan&age=25 HTTP/1.1\r\nHost: example.com\r\n\r\n");
    tests.push_back("POST /form HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 12\r\n\r\nname=Juan123");
    tests.push_back("POST /empty HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n");
    tests.push_back("GET / HTTP/1.1\r\nContent-Length: 0\r\nHost: example.com\r\n\r\n");
    tests.push_back("GET / HTTP/1.1\r\n\r\n");
    tests.push_back("GET test HTTP/1.1\r\nHost: example.com\r\n\r\n");
    tests.push_back("GET / HTTP/1.1\r\nHost example.com\r\n\r\n");
    tests.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\n123456789");
    tests.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: 10\r\n\r\n12345");
    tests.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\n\r\nhello");
    tests.push_back("GET /search?q=juan&&age=25 HTTP/1.1\r\nHost: example.com\r\n\r\n");
    tests.push_back("GET /search?q=hola\nmundo HTTP/1.1\r\nHost: example.com\r\n\r\n");
    tests.push_back("GET / HTTP/1.1\r\nHo\1st: example.com\r\n\r\n");
    tests.push_back("GET / HTTP/2.0\r\nHost: example.com\r\n\r\n");
    tests.push_back("GET / HTTP/1.1\r\nHost:    example.com\r\n\r\n");
    tests.push_back("GET / HTTP/1.1\r\nHost: example.com\r\nHost: evil.com\r\n\r\n");
    tests.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 5\r\n\r\n\r\n12345");
    tests.push_back("\r\n");
    tests.push_back("");

	for (size_t i = 0; i < tests.size(); ++i)
	{
    std::cout << "Test " << i << std::endl;

    Request req;
    RequestParser::parse(tests[i], req);

    if (RequestParser::valid_request(req))
	{
		//std::cout<<req<<std::endl;
        std::cout << "OK\n";
	}
    else
	{
			//std::cout<<req<<std::endl;
        std::cout << "ERROR " << req.get_status_code() << " " << req.get_final_status() << "\n";
	}


    std::cout << "-----------------\n";
}
	
}