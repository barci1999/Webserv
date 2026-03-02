#include"Block.hpp"
#include"Directive.hpp"
#include"Parser.hpp"
#include"server.hpp"

int main()
{
    try
    {
        Parser parser;
        Block root = parser.parseFile("hola.conf");
        const std::list<Block> hola = root.getBlocks();
        Block prueba(hola.begin());
        server general(prueba);
        std::cout << "hola se instancion bien la clase" << std::endl;

        Directive crocqueta;
        std::string *haa  = crocqueta.args.data();
        if (haa)
        {
            /* code */
        }
        
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}