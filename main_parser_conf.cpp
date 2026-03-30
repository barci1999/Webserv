#include"Parseo_solo_toca_Pablo/Block.hpp"
#include"Parseo_solo_toca_Pablo/Directive.hpp"
#include"Parseo_solo_toca_Pablo/Parser.hpp"
#include"Parseo_solo_toca_Pablo/server.hpp"

int main()
{
	try
	{
		Parser parser;
		Block root = parser.parseFile("Parseo_solo_toca_Pablo/hola.conf");
		const std::list<Block> hola = root.getBlocks();
		Block prueba(hola.begin());
		server general(prueba);
		std::cout << std::endl <<"hola se instancion bien la clase" << std::endl;
		prueba.print();

		Directive crocqueta;
		
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}