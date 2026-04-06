/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 21:20:08 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/06 17:32:48 by ksudyn           ###   ########.fr       */
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




class CGIProcess
{
	private:

		CGIProcess(){};
		~CGIProcess(){};
		int _inputPipe[2];
		int _outputPipe[2];
		pid_t _pid;

		std::string _cgiExtension;
		std::string _cgiPass;
		std::string _scriptPath;
		std::string _fullPath;

		std::string _buffer;
		bool _finished;

		void extractCGIConfig(const Block& best_location, const server server_config);
		std::string extractExtension(const std::string& path);

		std::string buildFullPath(const Request& request, const server& server_comfig);

		char **buildEnv(const Request& request);
		
		void createPipes();
		void forkProcess();
		void setupChildProcess(const Request& request);
		Response parseCGIResponse(const std::string& output);
	public:
		bool isCGI(const Request& request, const server& location);

		//std::string execute(const Request& request, const Block& location);
		void execute(const Request& request, const server& server_config);
		
		int getFD() const; //devuelve el fd para poll
		bool isFinished(); // lee sin bloquear
		void readFromPipe(); //comprueba si el CGI terminó
		Response buildResponse(); // construye la respuesta final
		
		const std::string& getBuffer() const;
		std::string handleRequest(Request& request, Block& location);//ESTA AL FINCLA DEL ARCHIVO .cpp

};


// class CGIProcess
// {
// 	private:

		// int _inputPipe[2];   // padre → hijo (stdin CGI)
		// int _outputPipe[2];  // hijo → padre (stdout CGI)

		// pid_t _pid;
		// bool _finished;
		// std::string _buffer;
		// std::string _cgiExtension;
		// std::string _cgiPass;
		// std::string _fullPath;
		// std::string _scriptPath;
		// void createPipes();
		// void forkProcess();
		// char **buildEnv(const Request& request);
		// void setupChildProcess(const Request& request);
		// Response parseCGIResponse(const std::string& output);

// 	public:


// };

// #endif