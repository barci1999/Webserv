/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 17:00:37 by ksudyn            #+#    #+#             */
/*   Updated: 2026/02/19 17:17:51 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>

class Request
{
    private:

    public:
        std::string method;
        std::string path;
        std::string query;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;

        Request() {};
};