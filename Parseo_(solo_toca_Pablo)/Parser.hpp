/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 01:10:42 by pablo             #+#    #+#             */
/*   Updated: 2026/03/05 14:24:39 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP
# include<string>
# include<fstream>
# include"Block.hpp"

enum tokens
{
    EMPTY,
    COMMENT,
    DIRECTIVE,
    BLOCK_START,
    BLOCK_END,
    ERROR,
};

class Parser
{
private:
    Block parseBlock(std::ifstream& file,const std::string& blockNmane); //metodo que nos retornara un nuevo objeto bloque para añadirlo al arbol
    tokens tokenize(const std::string& line); // metodo que nos devolvera un enum para comprobar el tipo de linea que se va a comprobar
    Directive parseDirective(const std::string& line);// metodo que nos retorna la linea parseada y separada 
public:
    Parser(/* args */);
    Parser(const Parser& other);
    Parser& operator=(const Parser& other);
    ~Parser();
    Block parseFile(const std::string& Filename);//nos retorna el arbol de bloques ya gestionados 
    static std::string trim(std::string input);
    static std::vector<std::string> str_to_vector(const std::string& to_change);
    static std::string vector_to_str(const std::vector<std::string> to_change);
};

#endif