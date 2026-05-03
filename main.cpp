/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 18:31:54 by pablalva          #+#    #+#             */
/*   Updated: 2026/05/03 19:01:05 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include "Logger.hpp"


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
		for (std::vector<server>::iterator it = root.begin(); it != root.end(); ++it)
		{
			log_server(it->get_srvHost().args[0],it->get_srvPorts());
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