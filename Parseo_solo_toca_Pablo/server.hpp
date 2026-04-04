/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 19:02:54 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/04 21:21:58 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*   Updated: 2026/03/05 13:05:26 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "Directive.hpp"
# include "Parser.hpp"
# include <vector>
# include <iostream>
# include <string>
# include "Block.hpp"
# include <exception>
# include <sstream>
# include <map>

class server
{
private:
//valores para konrad:

	std::string _srvName;
	Directive _srvPorts;
	Directive _srvRoot;
	Directive _srvIndex;
	Directive _srvErrorPage;
	std::list<Block> _srvLocations;
	size_t _srvClientMaxBody;
	Directive _srvAutoindex;

	
//valores para rodri:


	
	bool check_location_block(const Block& to_check);
	bool check_locations(const std::list<Block>);
	bool check_autoindex(const Block& to_check);
	size_t check_client_max_body(const Directive to_check);
	bool	cmp_name_directives(const Block& to_check);
	
	
	public:
	server(const Block& block);
	~server();
	server();
	
	static Directive check_directives(std::string to_serch, const Block& to_check);
	const	std::string& get_srvName()	const	{return this->_srvName;}
	const	Directive& get_srvPorts()	const	{return this->_srvPorts;}
	const	Directive& get_srvRoot()	const	{return this->_srvRoot;}
	const	Directive& get_srvIndex()	const	{return this->_srvIndex;}
	const	Directive& get_srvErrorPage()	const	{return this->_srvErrorPage;}
	const	std::list<Block>& get_srvLocations()	const	{return this->_srvLocations;}
	const	size_t& get_srvClientMaxBody()	const	{return this->_srvClientMaxBody;}
	const	Directive& get_srvAutoindex()	const {return this->_srvAutoindex;}
	
	void	set_srvName(std::string srvname)	{this->_srvName = srvname;}
	void	set_srvPorts(Directive& srvports)	{this->_srvPorts = srvports;}
	void	set_srvRoot(Directive& srvroot)	{this->_srvRoot = srvroot;}
	void	set_srvIndex(Directive& srvindex)	{this->_srvIndex = srvindex;}
	void	set_srvErrorPage(Directive& srverrorpage)	{this->_srvErrorPage = srverrorpage;}
	void	set_srvLocations(std::list<Block>& srvlocations)	{this->_srvLocations = srvlocations;}
	void	set_srvClientMaxBody(size_t srvclientmaxbody)	{this->_srvClientMaxBody = srvclientmaxbody;}
	void	set_srvAutoindex(Directive& srvautoindex)	{this->_srvAutoindex = srvautoindex;}

	Directive	take_concret_direc(std::string to_serch,std::vector<Directive> to_check);
	
	int 	insert_directives(const std::vector<Directive>& to_insert);
	int		insert_locations(const std::list<Block>& to_insert);
};
std::ostream& operator<<(std::ostream& out,const server& to_print);
#endif