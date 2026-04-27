/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollLoop.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by rodralva          #+#    #+#             */
/*   Updated: 2026/04/27 19:41:39 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pollLoop.hpp"

bool is_request_complete(const std::string& buffer)
{
    size_t header_end = buffer.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return false;

    std::string headers = buffer.substr(0, header_end);

    size_t pos = headers.find("Content-Length:");
    if (pos == std::string::npos)
        return true; // sin body → request completa

    pos += 15;

    while (pos < headers.size() && headers[pos] == ' ')
        pos++;

    size_t end_line = headers.find("\r\n", pos);
    std::string len_str = headers.substr(pos, end_line - pos);

    size_t content_length = std::atoi(len_str.c_str());

    size_t body_start = header_end + 4;
    size_t body_size = buffer.size() - body_start;

    return body_size >= content_length;
}
std::string extract_full_request(const std::string& buffer)
{
    size_t header_end = buffer.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return "";

    std::string headers = buffer.substr(0, header_end);

    size_t pos = headers.find("Content-Length:");
    if (pos == std::string::npos)
        return buffer; // no body

    pos += 15;

    while (pos < headers.size() && headers[pos] == ' ')
        pos++;

    size_t end_line = headers.find("\r\n", pos);
    std::string len_str = headers.substr(pos, end_line - pos);

    size_t content_length = std::atoi(len_str.c_str());

    size_t body_start = header_end + 4;

    return buffer.substr(0, body_start + content_length);
}

int pollLoop(std::vector<server> general)
{
    Directive srvPorts;
    std::vector<std::string> Ports;
    std::map<int,listener> srvListeners;
    std::map<int,client> srvClients;
    std::vector<pollfd> pollFds;
    pollfd fds;
    int eventFd;
    int active;

    for (std::vector<server>::iterator it = general.begin(); it!=general.end();it++){
        srvPorts=it->get_srvPorts();
        Ports=srvPorts.args;
        for (std::vector<std::string>::iterator it2 = Ports.begin(); it2!=Ports.end();it2++){
            listener temp(*it2);
            srvListeners.insert(std::make_pair(temp.get_lstSocket_fd(), temp));
        }
    }
    for (std::map<int,listener>::iterator it = srvListeners.begin(); it!=srvListeners.end();it++){
        //std::cout << it->second.get_lstPort() << std::endl;
        fds.fd = it->second.get_lstSocket_fd();
		fds.events = POLLIN;
		fds.revents = 0;
        //std::cout << "fds listeners " << fds.fd <<std::endl;
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

                //std::cout << "Nuevo cliente: " << client_fd << std::endl;
            }
            else
            {
				client& cl = srvClients[fd];
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
				cl.request.append(buffer,bytes);
				if (!is_request_complete(cl.request))
				{
					continue;
				}
				std::string full_request = extract_full_request(cl.request);
				cl.request.clear();

				Request req;
				//std::cout<<full_request<<std::endl;
				RequestParser::parse(full_request,req);
				RequestParser::valid_request(req);
				//std::cout<<">>>>>"<<std::endl;
				//std::cout<<req<<std::endl;
                
				
				Response resp = handleRequest(req,general[0]);
				//std::cout<<general[0]<<std::endl;
				//std::cout<<res_to_str(resp)<<std::endl;
				//std::cout << resp.get_statusCode()<<std::endl;
				std::string response_str = res_to_str(resp);
				send(fd, response_str.c_str(), response_str.size(), 0);
                //std::cout << "Datos recibidos de " << fd << std::endl;

                // aquí luego parsear HTTP
            }
        }
    }
    return (0);
}
