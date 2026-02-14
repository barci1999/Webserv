#include"Block.hpp"
#include"Directive.hpp"
#include"Parser.hpp"

int main()
{
    try
    {
        Parser parser;
        Block root = parser.parseFile("hola.conf");
        for(std::list<Block>::const_iterator it = root.getBlocks().begin(); it != root.getBlocks().end(); ++it)
        {
            std::cout << it->getName() << std::endl;
        }
        root.print();
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}