/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:28:51 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/07 14:40:12 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Parseo_solo_toca_Pablo/Directive.hpp"
#include "Parseo_solo_toca_Pablo/Block.hpp"
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include<sys/stat.h>
Directive search_directive(std::string to_cheack,const Block block);
std::string toString(unsigned int value);
bool is_directory(const std::string& path);
bool file_exist(const std::string file);
bool is_file(const std::string file);
bool can_read(const std::string file);
bool read_file(const std::string& path, std::string& out);