#include "Request.hpp"
#include "RequestParser.hpp"
#include "iostream"
#include <vector>


int main(void)
{

    std::vector<std::string> tests3;

    // tests.push_back("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n");
    // tests.push_back("GET /search?q=juan&age=25 HTTP/1.1\r\nHost: example.com\r\n\r\n");
    //tests.push_back("POST /form HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 12\r\n\r\nname=Juan123");
    // tests.push_back("POST /empty HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n");
    // tests.push_back("GET / HTTP/1.1\r\nContent-Length: 0\r\nHost: example.com\r\n\r\n");
    // tests.push_back("GET / HTTP/1.1\r\n\r\n");
    // tests.push_back("GET test HTTP/1.1\r\nHost: example.com\r\n\r\n");
    // tests.push_back("GET / HTTP/1.1\r\nHost example.com\r\n\r\n");
    // tests.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\n123456789");
    // tests.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: 10\r\n\r\n12345");
    // tests.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\n\r\nhello");
    //tests.push_back("GET /search?q=juan&&age=25 HTTP/1.1\r\nHost: example.com\r\n\r\n");
    // tests.push_back("GET /search?q=hola\nmundo HTTP/1.1\r\nHost: example.com\r\n\r\n");
    // tests.push_back("GET / HTTP/1.1\r\nHo\1st: example.com\r\n\r\n");
    // tests.push_back("GET / HTTP/2.0\r\nHost: example.com\r\n\r\n");
    // tests.push_back("GET / HTTP/1.1\r\nHost:    example.com\r\n\r\n");
    // tests.push_back("GET / HTTP/1.1\r\nHost: example.com\r\nHost: evil.com\r\n\r\n");
    // tests.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 5\r\n\r\n\r\n12345");
    // tests.push_back("\r\n");
    // tests.push_back("");
// tests2.push_back("GET /path?key=value HTTP/1.1\r\nHost: example.com\r\n\r\n");  

// tests2.push_back("GET /path?key= HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET /path?key HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET /path?key=value1&key=value2 HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET /path?=value HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET /path?&&& HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET /path?key=value%20encoded HTTP/1.1\r\nHost: example.com\r\n\r\n");

// // ❌ inválidos query
// tests2.push_back("GET /path?key=value\nbad HTTP/1.1\r\nHost: example.com\r\n\r\n");

// // ❌ headers

// tests2.push_back("GET / HTTP/1.1\r\nHost:\r\n\r\n"); // host vacion

// tests2.push_back("GET / HTTP/1.1\r\nHost: \r\n\r\n"); //host solo espacios

// tests2.push_back("GET / HTTP/1.1\r\nHost: example.com\r\nContent-Length: abc\r\n\r\n");// content lenth no numerico

// //❌ body / content-length
// tests2.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: -5\r\n\r\nhello");

// tests2.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\nhello");

// tests2.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nhelloEXTRA");

// // ❌ request line
// tests2.push_back("INVALID / HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET  HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET /");

// // 🔥 tricky
// tests2.push_back("GET /path?key=value=value2 HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET /path?= HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests2.push_back("GET /path?key1=value1&=value2&key3=value3 HTTP/1.1\r\nHost: example.com\r\n\r\n");
// tests3.push_back("GET /../../etc/passwd HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests3.push_back("GET /images/../secret.txt HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests3.push_back("GET /a/b/../../c HTTP/1.1\r\nHost: example.com\r\n\r\n");


// // 🔥 ENCODING (muy importante)

// tests3.push_back("GET /search?q=hola%20mundo HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests3.push_back("GET /search?q=%2e%2e%2f HTTP/1.1\r\nHost: example.com\r\n\r\n"); // ../ encoded

// tests3.push_back("GET /%2e%2e/%2e%2e/etc/passwd HTTP/1.1\r\nHost: example.com\r\n\r\n");


// // 🔥 HEADERS RAROS

// tests3.push_back("GET / HTTP/1.1\r\nHost:example.com\r\n\r\n"); // sin espacio

// tests3.push_back("GET / HTTP/1.1\r\nHost:    example.com\r\n\r\n"); // muchos espacios

// tests3.push_back("GET / HTTP/1.1\r\nHost: example.com:8080\r\n\r\n"); // con puerto

//tests3.push_back("GET / HTTP/1.1\r\nhost: example.com\r\n\r\n"); // lowercase


// 🔥 MULTILINE / SPACES

// tests3.push_back("GET    /path    HTTP/1.1\r\nHost: example.com\r\n\r\n");

// tests3.push_back("GET /path HTTP/1.1\r\nHost: example.com\r\n   \r\n"); // línea rara


// 🔥 BODY EDGE CASES

// tests3.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 5\r\n\r\n12345");

// tests3.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 5\r\n\r\n1234"); // menos body

// tests3.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 5\r\n\r\n123456"); // más body


// 🔥 HEADERS DUPLICADOS IMPORTANTES

// tests3.push_back("POST /test HTTP/1.1\r\nHost: example.com\r\nContent-Length: 5\r\nContent-Length: 10\r\n\r\n12345");


// // 🔥 REQUEST EXTRAÑA

// tests3.push_back("GET /path HTTP/1.1\r\nHost: example.com\r\nExtra: value:with:colons\r\n\r\n");


// 🔥 MUY IMPORTANTE (CRLF injection)

tests3.push_back("GET /path HTTP/1.1\r\nHost: example.com\r\nX-Test: hello\rmalicious\r\n\r\n");


// 🔥 CASO LÍMITE

tests3.push_back("GET /" + std::string(3000, 'a') + " HTTP/1.1\r\nHost: example.com\r\n\r\n");
	for (size_t i = 0; i < tests3.size(); ++i)
	{
    std::cout << "Test " << i << std::endl;

    Request req;
    RequestParser::parse(tests3[i], req);

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