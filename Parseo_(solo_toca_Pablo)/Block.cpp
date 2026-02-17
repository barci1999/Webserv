/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Block.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 01:45:12 by pablo             #+#    #+#             */
/*   Updated: 2026/02/17 17:24:24 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Block.hpp"

Block::Block(){}
Block::Block(const Block& other)
{
	this->_name = other._name;
	this->_directives = other._directives;
	this->_children = other._children;
}
Block& Block::operator=(const Block& other)
{
	if (this == &other)
	{
		return *this;
	}
	this->_name = other._name;
	this->_children = other._children;
	this->_directives = other._directives;
	return *this;
}
Block::~Block(){}

Block::Block(const std::string name)
{
	this->_name = name;
}
void Block::addDirective(const Directive& to_add)
{
	this->_directives.push_back(to_add);
}
void Block::addChild(const Block& to_add)
{
	this->_children.push_back(to_add);
}
const std::string& Block::getName() const
{
	return this->_name;
}

const std::vector<Directive>& Block::getDirectives() const
{
	return  this->_directives;
}
const std::list<Block>& Block::getBlocks()
{
	return this->_children;
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

    for (std::list<Block>::const_iterator it = _children.begin(); it != _children.end(); ++it)
        it->print(indent + 1);
}