/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pollLoop.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:21:33 by rodralva          #+#    #+#             */
/*   Updated: 2026/05/04 21:10:51 by ksudyn           ###   ########.fr       */
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
    std::map<int, listener> srvListeners;
    std::map<int, client> srvClients;
    std::vector<pollfd> pollFds;

    // 🔥 SISTEMA DE GESTIÓN ASÍNCRONA
    std::map<int, CGIProcess*> cgi_map;      // pipe_fd -> proceso
    std::map<int, int> cgi_to_client;        // pipe_fd -> client_fd
    std::map<int, Request> pending_requests; // fd_objetivo -> Request (para el log)
    std::map<int, std::string> out_buffers;  // client_fd -> string a enviar

    try {
        for (size_t i = 0; i < general.size(); ++i) {
            Directive srvPorts = general[i].get_srvPorts();
            for (size_t j = 0; j < srvPorts.args.size(); ++j) {
                listener temp(srvPorts.args[j], general[i]);
                temp.set_originalsrv(&general[i]);
                int l_fd = temp.get_lstSocket_fd();
                srvListeners.insert(std::make_pair(l_fd, temp));
                pollfd fds = {l_fd, POLLIN, 0};
                pollFds.push_back(fds);
            }
        }
    } catch (const std::exception& e) { return 1; }

    while (Signal::runnin == 1) {
        int active = poll(pollFds.data(), pollFds.size(), -1);
        if (active < 0) break;

        for (size_t i = 0; i < pollFds.size(); i++) {
            int fd = pollFds[i].fd;
            short rev = pollFds[i].revents;
            if (rev == 0) continue;

            // --- 1. ENVIAR RESPUESTAS (ÚNICO PUNTO DE SEND) ---
            if (rev & POLLOUT) {
                if (out_buffers.count(fd)) {
                    send(fd, out_buffers[fd].c_str(), out_buffers[fd].size(), 0);
                    
                    // Loguear usando la request que guardamos
                    if (pending_requests.count(fd)) {
                        Request& req = pending_requests[fd];
                        log_request(req.get_method(), req.get_path(), req.get_version(), 200);
                        pending_requests.erase(fd);
                    }
                    
                    out_buffers.erase(fd);
                }
                pollFds[i].events &= ~POLLOUT; // Quitar interés en escribir
                continue;
            }

            // --- 2. MANEJO DE CGI ---
            if (cgi_map.count(fd)) {
                CGIProcess* cgi = cgi_map[fd];
                if (rev & POLLIN) cgi->readFromPipe();
                if (rev & (POLLHUP | POLLERR)) {
                    cgi->readFromPipe();
                    int client_fd = cgi_to_client[fd];
                    
                    // Preparar respuesta para el "send" centralizado
                    out_buffers[client_fd] = res_to_str(cgi->buildResponse());
                    
                    // Activar POLLOUT en el cliente
                    for (size_t j = 0; j < pollFds.size(); ++j)
                        if (pollFds[j].fd == client_fd) pollFds[j].events |= POLLOUT;

                    close(fd);
                    delete cgi;
                    cgi_map.erase(fd);
                    cgi_to_client.erase(fd);
                    pollFds.erase(pollFds.begin() + i--);
                }
                continue;
            }

            // --- 3. NUEVAS CONEXIONES ---
            if (srvListeners.count(fd)) {
                int c_fd = accept(fd, NULL, NULL);
                if (c_fd >= 0) {
                    pollfd n_p = {c_fd, POLLIN, 0};
                    pollFds.push_back(n_p);
                    client clnt(c_fd);
                    clnt.set_ptr(&srvListeners.at(fd));
                    srvClients[c_fd] = clnt;
                }
                continue;
            }

            // --- 4. PETICIONES DE CLIENTES ---
            if (srvClients.count(fd)) {
                if (rev & (POLLHUP | POLLERR)) {
                    close(fd);
                    srvClients.erase(fd);
                    pending_requests.erase(fd);
                    out_buffers.erase(fd);
                    pollFds.erase(pollFds.begin() + i--);
                    continue;
                }

                client& cl = srvClients[fd];
                char buf[4096];
                int b = read(fd, buf, sizeof(buf));
                if (b <= 0) { /* cerrar igual que arriba */ continue; }

                cl.request.append(buf, b);
                server s_ptr = cl.get_ptr()->get_originalsrv();
                if (is_request_complete(cl.request, s_ptr) == INCOMPLETE) continue;

                Request req;
                RequestParser::parse(extract_full_request(cl.request), req);
                RequestParser::valid_request(req);
                cl.request.clear();

                Response resp;
                bool ready = handleRequest(req, s_ptr, pollFds, cgi_map, cgi_to_client, fd, resp);

                // Guardamos la request para el log futuro (sea CGI o no)
                pending_requests[fd] = req;

                if (ready) {
                    out_buffers[fd] = res_to_str(resp);
                    pollFds[i].events |= POLLOUT;
                }
            }
        }
    }
    return 0;
}
