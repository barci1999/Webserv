/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Directive.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 01:11:31 by pablo             #+#    #+#             */
/*   Updated: 2026/02/11 01:44:53 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTIVE_HPP
# define DIRECTIVE_HPP
# include<vector>
# include<iostream>
struct Directive //struct que consta de un vector de string que se usa en parser y block para ahorrar espacio.
{
    std::vector<std::string> args;
};
#endif