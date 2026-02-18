/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProces.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 16:55:57 by ksudyn            #+#    #+#             */
/*   Updated: 2026/02/18 16:58:33 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "unistd.h"

class CGIProces
{
    private:
        int _inputPipe[2];
        int _uotputPippe[2];
        pid_t _pid;
    public:
        pid_t getPid() const;
};
