/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 21:20:08 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/16 12:54:52 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "unistd.h"
#include <fstream>
#include <sstream>
#include "RequestParser.hpp"
#include "Block.hpp"

class CGIProcess
{
	private:
		int _inputPipe[2];
		int _uotputPippe[2];
		pid_t _pid;

		std::string _cgiExtension;
		std::string _cgiPass;
		std::string _scriptPath;
		std::string _fullPath;

		void extractCGIConfig(const Block& location);
		std::string extractExtension(const std::string& path);

		std::string buildFullPath(const Request& request, const Block& location);

		void createPipes();
		void forkProcess();
		void setupChildProcess(const Request& request);
		std::string handleParentProcess(const Request& request);
	public:
		bool isCGI(const Request& request, const Block& location);

		std::string execute(const Request& request, const Block& location);

		std::string serveStaticFile(const Request& request, const Block& location);
		std::string handleRequest(Request& request, Block& location);

		pid_t getPid() const;
};

