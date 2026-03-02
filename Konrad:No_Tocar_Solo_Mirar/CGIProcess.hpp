/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 21:20:08 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/02 21:20:27 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "unistd.h"
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

        void extractCGIConfig(const Block& location);
        std::string extractExtension(const std::string& path);
    public:
        bool isCGI(const Request& request, const Block& location);
        pid_t getPid() const;
};

