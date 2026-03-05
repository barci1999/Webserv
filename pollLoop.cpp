/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollLoop.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by rodralva          #+#    #+#             */
/*   Updated: 2026/03/04 14:29:37 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pollLoop.hpp"
#include "./Parseo_(solo_toca_Pablo)/Block.hpp"
#include "./Parseo_(solo_toca_Pablo)/Directive.hpp"
#include "./Parseo_(solo_toca_Pablo)/Parser.hpp"
#include "./Parseo_(solo_toca_Pablo)/server.hpp"
#include "client.hpp"
#include "listener.hpp"
#include "poll.h"

//int pollLoop(server Webservconf)
int pollLoop(void)
{
    Directive srvPorts;
    std::vector<std::string> Ports;
    std::vector<listener> srvListeners;
    std::map<int,client> srvClients;
    std::vector<pollfd> pollFds;
    pollfd fds;
    int eventFd;

    Ports.push_back("8080");
    Ports.push_back("8082");
    Ports.push_back("8085");
    Ports.push_back("8089");
    //srvPorts=Webservconf.get_srvPorts();
    //Ports=srvPorts.args;
    for (std::vector<std::string>::iterator it = Ports.begin(); it!=Ports.end();it++){
        srvListeners.push_back(listener(*it));
    }
    for (std::vector<listener>::iterator it = srvListeners.begin(); it!=srvListeners.end();it++){
        std::cout << it->get_lstPort() << std::endl;
        fds.fd = it->get_lstSocket_fd();
        std::cout << "fds listeners " << fds.fd <<std::endl;
        pollFds.push_back(fds);
    }
    while (1)
    {
        eventFd=poll(pollFds.data(), pollFds.size(),5000);
        std::cout << "evet " << eventFd << std::endl;
        if (srvClients.find(eventFd) != srvClients.end())
            std::cout << "hola" << std::endl;
        else
        {
            sockaddr_in test;
            int test_socketfd;
            pollfd test_poll;
            for (std::vector<listener>::iterator it = srvListeners.begin(); it!=srvListeners.end();it++){
                if (it->get_lstSocket_fd() == eventFd){
                    test_socketfd=it->get_lstSocket_fd();
                    std::cout << "tests socket fd " << test_socketfd << std::endl;
                    test=it->get_lstSocketAddr();
                }
            }
            socklen_t client_len = sizeof(test);
            client clnt(accept(test_socketfd,(struct sockaddr*)&test,&client_len));
            srvClients[clnt.get_fd()] = clnt;
            test_poll.fd=clnt.get_fd();
            std::cout << "client_fd " << test_poll.fd << std::endl;
            pollFds.push_back(test_poll);
        }
    }
    return (0);
}

// int main()
// {
//     pollLoop();

//     return(0);
// }