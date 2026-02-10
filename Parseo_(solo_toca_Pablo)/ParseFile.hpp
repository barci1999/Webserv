/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseFile.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 20:07:01 by pablalva          #+#    #+#             */
/*   Updated: 2026/02/10 21:11:43 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>
#include<iostream>
#include<file_block.hpp>
# include<list>

class ParseFile
{
private:
	
	std::map<std::string,std::string> _def;
	std::list<ParseFile> _blocks;
public:
	ParseFile(/* args */);
	~ParseFile();
};

ParseFile::ParseFile(/* args */)
{
}

ParseFile::~ParseFile()
{
}
