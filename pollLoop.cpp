/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollLoop.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by rodralva          #+#    #+#             */
/*   Updated: 2026/03/16 12:53:14 by pablalva         ###   ########.fr       */
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


int pollLoop(server general)
{
    Directive srvPorts;
    std::vector<std::string> Ports;
    std::vector<listener> srvListeners;
    std::map<int,client> srvClients;
    std::vector<pollfd> pollFds;
    pollfd fds;
    int eventFd;
    int active;

    srvPorts=general.get_srvPorts();
    Ports=srvPorts.args;
    for (std::vector<std::string>::iterator it = Ports.begin(); it!=Ports.end();it++){
        srvListeners.push_back(listener(*it));
    }
    for (std::vector<listener>::iterator it = srvListeners.begin(); it!=srvListeners.end();it++){
        std::cout << it->get_lstPort() << std::endl;
        fds.fd = it->get_lstSocket_fd();
        std::cout << "fds listeners " << fds.fd <<std::endl;
        pollFds.push_back(fds);
    }
    while (true)
    {
        int active = poll(pollFds.data(), pollFds.size(), -1);

        if (active < 0)
        {
            perror("poll");
            break;
        }
        for (size_t i = 0; i < pollFds.size(); i++)
        {
            if (pollFds[i].revents == 0)
                continue;
            int fd = pollFds[i].fd;
            if (pollFds[i].revents & (POLLHUP | POLLERR))
            {
                close(fd);
                srvClients.erase(fd);
                pollFds.erase(pollFds.begin() + i);
                i--;
                continue;
            }
            if (srvClients.find(fd) == srvClients.end())
            {
                int client_fd = accept(fd, NULL, NULL);
                if (client_fd < 0)
                    continue;

                client clnt(client_fd);
                srvClients[client_fd] = clnt;

                pollfd new_poll;
                new_poll.fd = client_fd;
                new_poll.events = POLLIN;
                new_poll.revents = 0;

                pollFds.push_back(new_poll);

                std::cout << "Nuevo cliente: " << client_fd << std::endl;
            }
            else
            {
                char buffer[1024];
                int bytes = read(fd, buffer, sizeof(buffer));

                if (bytes <= 0)
                {
                    close(fd);
                    srvClients.erase(fd);
                    pollFds.erase(pollFds.begin() + i);
                    i--;
                    continue;
                }

                std::cout << "Datos recibidos de " << fd << std::endl;

                // aquí luego parsear HTTP
            }
        }
    }
    return (0);
}

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
         pollLoop(general);
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
