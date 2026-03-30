#include <iostream>
#include <fstream>
#include "Parseo_solo_toca_Pablo/Directive.hpp"
#include "Parseo_solo_toca_Pablo/Block.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Request.hpp"
#include "Konrad_No_Tocar_Solo_Mirar/Response.hpp"
#include <stdlib.h>

int main()
{
    // 1️⃣ Crear carpeta y archivo de prueba
    system("mkdir -p test");
    std::ofstream("test/index.html") << "<h1>Hola mundo!</h1>";

    // 2️⃣ Configurar Request
    Request req("/test/"); // suponiendo que make_Get asume GET

    // 3️⃣ Configurar Block
    Block server("test");

    // Crear root directive
    Directive root;
    root.name = "root";
    root.args.push_back("."); // carpeta actual
    server.addDirective(root);

    // Crear index directive
    Directive index;
    index.name = "index";
    index.args.push_back("index.html");
    server.addDirective(index);

    // 4️⃣ Crear Response
    Response res;
    res.make_Get(req, server);

    // 5️⃣ Imprimir resultado para verificar
    std::cout << "HTTP version: " << res.get_version() << "\n";
    std::cout << "Status code: " << res.get_statusCode() << "\n";
    std::cout << "Reason phrase: " << res.get_reasonPhrase() << "\n";
    std::cout << "Headers:\n";
    std::map<std::string,std::string> headers = res.get_headers();
    for (std::map<std::string,std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
        std::cout << it->first << ": " << it->second << "\n";
    std::cout << "Body:\n" << res.get_body() << "\n";

    return 0;
}