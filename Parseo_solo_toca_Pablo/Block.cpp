/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Block.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 01:45:12 by pablo             #+#    #+#             */
/*   Updated: 2026/04/05 16:36:58 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Block.hpp"
#include "Parser.hpp"

Block::Block(){}
Block::Block(const Block& other)
{
	this->_name = other._name;
	this->_directives = other._directives;

}
Block& Block::operator=(const Block& other)
{
	if (this == &other)
	{
		return *this;
	}
	this->_name = other._name;

	this->_directives = other._directives;
	return *this;
}
Block::Block(std::list<Block>::const_iterator hola)
{
	this->_name = hola->getName();
	this->_directives = hola->getDirectives();

}
Block::~Block(){}

Block::Block(const std::string name)
{
	this->_name = name;
}
void Block::addDirective(const Directive& to_add)
{
	
	for (std::vector<Directive>::iterator it = this->_directives.begin(); it != this->_directives.end(); it++)
	{
		if (it->name == to_add.name)
		{
			std::string excep = "Duplicate directive detected -> " + to_add.name + " " + Parser::vector_to_str(to_add.args);
			throw std::runtime_error(excep.c_str());
		}
		
	}
	
	this->_directives.push_back(to_add);
}

const std::string& Block::getName() const
{
	return this->_name;
}

const std::vector<Directive>& Block::getDirectives() const
{
	return  this->_directives;
}
void Block::print(int indent) const
{
	for (int i = 0; i < indent; ++i) std::cout << "  ";
	std::cout << "Block: " << _name << std::endl;

	for (size_t i = 0; i < _directives.size(); ++i)
	{
		for (int j = 0; j < indent + 1; ++j) std::cout << "  ";
		for (size_t k = 0; k < _directives[i].args.size(); ++k)
			std::cout << _directives[i].args[k] << " ";
		std::cout << std::endl;
	}
}