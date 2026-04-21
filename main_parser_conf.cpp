#include"Parseo_solo_toca_Pablo/Block.hpp"
#include"Parseo_solo_toca_Pablo/Directive.hpp"
#include"Parseo_solo_toca_Pablo/Parser.hpp"
#include"Parseo_solo_toca_Pablo/server.hpp"

int main()
{
	try
	{
		Parser parser;
		std::vector<server> Servers = parser.parseFile("conf_test_response.txt");
		for (std::vector<server>::iterator it = Servers.begin(); it != Servers.end(); ++it)
		{
			std::cout<<*it<<std::endl;
		}
		
		std::cout << std::endl <<"hola se instancion bien la clase" << std::endl;

		Directive crocqueta;
		
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}