/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Block.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 01:14:57 by pablo             #+#    #+#             */
/*   Updated: 2026/02/17 17:27:52 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BLOCK_HPP
# define BLOCK_HPP
# include<string>
# include<vector>
# include<list>
# include"Directive.hpp"

class Block
{
private:
    std::string _name;//nombre del bloque
    std::vector<Directive> _directives;//conjunto de directivas
    std::list<Block> _children;//conjunto de subbloques que se encuentran dentro del actual.
public:
    Block(/* args */);
    Block(const Block& other);
    Block& operator=(const Block& other);
    ~Block();
    
    Block(const std::string name);//costructor de la clae blocke donde el nombre sera el titulo del blocque
    
    void addDirective(const Directive& to_add);//se usara para añadir una directive al vector de la clase.
    void addChild(const Block& to_add);//se usara para añadir un nuevo subbloque dentro del actual despues se resolvera todo de forma recursiva.
    
    const std::string& getName() const;
    const std::vector<Directive>& getDirectives() const;
    const std::list<Block>& getBlocks() const;
    
    

    void print(int indent = 0) const;
};


#endif