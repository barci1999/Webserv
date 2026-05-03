/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollLoop.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by rodralva          #+#    #+#             */
/*   Updated: 2026/05/03 19:30:14 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pollLoop.hpp"
#include <csignal>
#include "signal.hpp"
#include <limits.h>
#include "Logger.hpp"

RequestStatus is_request_complete(const std::string& buffer, const server& Server)
{
    // 1. Buscar el final de los encabezados
    size_t header_end = buffer.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return INCOMPLETE;

    std::string headers = buffer.substr(0, header_end);

    if (headers.find("Transfer-Encoding: chunked") != std::string::npos)
    {

        if (buffer.find("\r\n0\r\n\r\n", header_end) != std::string::npos)
            return COMPLETE;
        if (buffer.size() - (header_end + 4) > Server.get_srvClientMaxBody())
            return TOO_LARGE;

        return INCOMPLETE;
    }
    size_t pos = headers.find("Content-Length:");
    if (pos == std::string::npos)
    {
        return COMPLETE;
    }

    pos += 15;
    size_t end_line = headers.find("\r\n", pos);
    if (end_line == std::string::npos) return MALFORMED;

    std::string len_str = headers.substr(pos, end_line - pos);

    if (len_str.empty() || len_str.find_first_not_of(" 0123456789") != std::string::npos)
        return MALFORMED;

    long content_length = std::atol(len_str.c_str());

    // Validar límites y valores negativos
    if (content_length < 0)
        return MALFORMED;

    if (static_cast<size_t>(content_length) > Server.get_srvClientMaxBody())
        return TOO_LARGE;

    // 4. Comprobar si el cuerpo recibido coincide con lo esperado
    size_t body_start = header_end + 4;
    size_t bytes_received = buffer.size() - body_start;

    if (bytes_received >= static_cast<size_t>(content_length))
        return COMPLETE;

    return INCOMPLETE;
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
    int i = 0;
	try
	{
		for (std::vector<server>::iterator it = general.begin(); it!=general.end();it++){
			srvPorts=it->get_srvPorts();
			Ports=srvPorts.args;
			for (std::vector<std::string>::iterator it2 = Ports.begin(); it2!=Ports.end();it2++){
				listener temp(*it2,*it);
				temp.set_originalsrv(&general[i]);
				srvListeners.insert(std::make_pair(temp.get_lstSocket_fd(), temp));
			}
			i++;
		}
		for (std::map<int,listener>::iterator it = srvListeners.begin(); it!=srvListeners.end();it++){
			fds.fd = it->second.get_lstSocket_fd();
			fds.events = POLLIN;
			fds.revents = 0;
			pollFds.push_back(fds);
		}
	}
	catch(const std::invalid_argument& e)
	{
		std::cerr << e.what() << '\n';
		for (std::map<int,listener>::iterator it = srvListeners.begin(); it != srvListeners.end(); ++it)
		{
			close(it->second.get_lstSocket_fd());
		}
		Signal::runnin = 0;	
	}
    while (Signal::runnin == 1)
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
                std::map<int, listener>::iterator it = srvListeners.find(fd);
                if (it != srvListeners.end())
                {
                    srvClients[client_fd].set_ptr(&it->second);
                }
                pollfd new_poll;
                new_poll.fd = client_fd;
                new_poll.events = POLLIN;
                new_poll.revents = 0;

                pollFds.push_back(new_poll);
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
				RequestStatus status = is_request_complete(cl.request,cl.get_ptr()->get_originalsrv());
				Request req;
				if(status == INCOMPLETE) continue;
				if (status == MALFORMED)
				{
					close(fd);
					req.set_status_code(400);
					req.set_final_status("Bad Request");
				}
				if (status == TOO_LARGE)
				{
					close(fd);
					req.set_status_code(413);
					req.set_final_status("Payload Too Large");
				}

				std::string full_request = extract_full_request(cl.request);
				cl.request.clear();
                const listener *tmp = cl.get_ptr();
				RequestParser::parse(full_request,req);
				RequestParser::valid_request(req);
				Response resp = handleRequest(req,tmp->get_originalsrv());
				std::string response_str = res_to_str(resp);
				send(fd, response_str.c_str(), response_str.size(), 0);
				log_request(req.get_method(),req.get_path(),req.get_version(),resp.get_statusCode());
				
            }
        }
    }
	for (std::map<int,listener>::iterator it = srvListeners.begin(); it != srvListeners.end(); ++it)
	{
		close(it->second.get_lstSocket_fd());
	}
    return (0);
}
