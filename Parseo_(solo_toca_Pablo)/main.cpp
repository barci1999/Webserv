#include"Block.hpp"
#include"Directive.hpp"
#include"Parser.hpp"

int main()
{
    try
    {
        Parser parser;
        Block root = parser.parseFile("prueba.conf");
        root.print();
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}