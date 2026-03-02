/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollLoop.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rodralva <rodralva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by rodralva          #+#    #+#             */
/*   Updated: 2026/02/18 15:50:35 by rodralva         ###   ########.fr       */
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
        pollFds.push_back(fds);
    }
    while (1)
    {
        eventFd=poll(pollFds.data(), pollFds.size(),500);
        if (srvClients.find(eventFd) != srvClients.end())
            std::cout << "hola" << std::endl;
        else
            std::cout << "else" << std::endl; 

    }
    return (0);
}

int main()
{
    pollLoop();

    return(0);
}