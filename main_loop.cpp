#include "pollLoop.hpp"
#include "./Parseo_solo_toca_Pablo/Block.hpp"
#include "./Parseo_solo_toca_Pablo/Directive.hpp"
#include "./Parseo_solo_toca_Pablo/Parser.hpp"
#include "./Parseo_solo_toca_Pablo/server.hpp"
#include "client.hpp"
#include "listener.hpp"
#include "poll.h"
#include <unistd.h>

 
 int main()
 {
     try
     {
         Parser parser;
         std::vector<server> root = parser.parseFile("conf_curl_test.txt");
         pollLoop(root);
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