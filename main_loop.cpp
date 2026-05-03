#include "pollLoop.hpp"
#include "./Parseo_solo_toca_Pablo/Block.hpp"
#include "./Parseo_solo_toca_Pablo/Directive.hpp"
#include "./Parseo_solo_toca_Pablo/Parser.hpp"
#include "./Parseo_solo_toca_Pablo/server.hpp"
#include "client.hpp"
#include "listener.hpp"
#include "poll.h"
#include <unistd.h>
#include "signal.hpp"


int main(int argc ,char **argv)
{
	Signal::reginster_singal();
    try
    {
        Parser parser;
		if (argc > 2)
		{
			throw std::runtime_error("Invalid number of arguments");
		}
		std::vector<server> root;
		if (argc == 2)
		{
			root = parser.parseFile(std::string(argv[1]));
		}
		else
		{
			root = parser.parseFile("conf_curl_test.txt");
		}
		pollLoop(root);
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
	}
    return 0;
}