/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 21:20:08 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/05 11:47:33 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <iostream>

#include "RequestParser.hpp"
#include "Response.hpp"
#include "../Parseo_solo_toca_Pablo/server.hpp"
#include "../utils.hpp"
#include "poll.h"




class CGIProcess
{
	private:

		int _inputPipe[2];
		int _outputPipe[2];
		pid_t _pid;

		std::string _cgiPass;
		std::string _scriptPath;
		std::string _fullPath;

		std::string _buffer;
		bool _finished;

		void extractCGIConfig(const Block& best_location, const server& server_config);

		std::string buildFullPath(const Request& request, const server& server_config);

		char **buildEnv(const Request& request);
		
		void createPipes();
		void forkProcess();
		void setupChildProcess(const Request& request);
		Response parseCGIResponse(const std::string& output);
	public:
		CGIProcess(){};
		~CGIProcess(){};
		bool isCGI(const Request& request, const server& location);

		void execute(const Request& request, const server& server_config,std::vector<pollfd>& pollFds);
		
		int getFD() const; //devuelve el fd para poll
		bool isFinished(); // lee sin bloquear
		void readFromPipe(); //comprueba si el CGI terminó
		Response buildResponse(); // construye la respuesta final
		
		std::map<std::string, std::string> _cgimap;
};
