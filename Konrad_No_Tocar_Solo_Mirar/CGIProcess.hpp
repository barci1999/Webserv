/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 21:20:08 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/30 18:13:35 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "unistd.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/wait.h>
#include <cstdlib>
# include <fcntl.h>
#include "RequestParser.hpp"
#include "../Parseo_solo_toca_Pablo/Block.hpp"
#include "Response.hpp"

class CGIProcess
{
	private:
		int _inputPipe[2];
		int _outputPipe[2];
		pid_t _pid;

		std::string _cgiExtension;
		std::string _cgiPass;
		std::string _scriptPath;
		std::string _fullPath;

		std::string _buffer;
		bool _finished;

		void extractCGIConfig(const Block& location);
		std::string extractExtension(const std::string& path);

		std::string buildFullPath(const Request& request, const Block& location);

		char **buildEnv(const Request& request);
		
		void createPipes();
		void forkProcess();
		void setupChildProcess(const Request& request);
		std::string handleParentProcess(const Request& request);
		Response parseCGIResponse(const std::string& output);
	public:
		bool isCGI(const Request& request, const Block& location);

		//std::string execute(const Request& request, const Block& location);
		void execute(const Request& request, const Block& location);
		
		std::string serveStaticFile(const Request& request, const Block& location);
		std::string handleRequest(Request& request, Block& location);

		pid_t getPid() const;

		int getFD() const; //devuelve el fd para poll
		bool isFinished(); // lee sin bloquear
		void readFromPipe(); //comprueba si el CGI terminó
		Response buildResponse(); // construye la respuesta final

};
