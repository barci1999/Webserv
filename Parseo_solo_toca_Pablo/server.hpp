/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 19:02:54 by pablalva          #+#    #+#             */
/*   Updated: 2026/04/05 17:46:04 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*   Updated: 2026/03/05 13:05:26 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP
# include "Directive.hpp"
# include <vector>
# include <iostream>
# include <string>
# include "Block.hpp"
# include <exception>
# include <sstream>
# include <map>
# include <stdlib.h>
# include <set>
class server
{
private:
//valores para konrad:

	std::string _srvName;
	Directive _srvPorts;
	Directive _srvRoot;
	Directive _srvIndex;
	std::vector<Directive> _srvErrorPage;
	std::list<Block> _srvLocations;
	size_t _srvClientMaxBody;
	Directive _srvAutoindex;

	
//valores para rodri:


	
	bool check_location_block(const Block& to_check);
	bool check_locations(const std::list<Block>);
	Directive check_autoindex(const Directive& to_check);
	size_t check_client_max_body(const Directive to_check);
	void validate_directives_names(const Block& to_check);
	Directive check_index(const Directive& to_check);
	Directive check_error_page(const Directive& to_check);
	Directive check_root(const Directive& to_check);
	Directive check_listen(const Directive& to_check);
	
	
	public:
	server();
	~server();
	
	static Directive check_directives(std::string to_serch, const Block& to_check);
	const	std::string& get_srvName()	const	{return this->_srvName;}
	const	Directive& get_srvPorts()	const	{return this->_srvPorts;}
	const	Directive& get_srvRoot()	const	{return this->_srvRoot;}
	const	Directive& get_srvIndex()	const	{return this->_srvIndex;}
	const	std::vector<Directive>& get_srvErrorPage()	const	{return this->_srvErrorPage;}
	const	std::list<Block>& get_srvLocations()	const	{return this->_srvLocations;}
	const	size_t& get_srvClientMaxBody()	const	{return this->_srvClientMaxBody;}
	const	Directive& get_srvAutoindex()	const {return this->_srvAutoindex;}
	
	void	set_srvName(std::string srvname)	{this->_srvName = srvname;}
	void	set_srvPorts(Directive& srvports)	{this->_srvPorts = srvports;}
	void	set_srvRoot(Directive& srvroot)	{this->_srvRoot = srvroot;}
	void	set_srvIndex(Directive& srvindex)	{this->_srvIndex = srvindex;}
	void	set_srvErrorPage(Directive& srverrorpage)	{this->_srvErrorPage.push_back(srverrorpage);}
	void	set_srvLocations(std::list<Block>& srvlocations)	{this->_srvLocations = srvlocations;}
	void	set_srvClientMaxBody(size_t srvclientmaxbody)	{this->_srvClientMaxBody = srvclientmaxbody;}
	void	set_srvAutoindex(Directive& srvautoindex)	{this->_srvAutoindex = srvautoindex;}

	Directive	take_concret_direc(const std::string& to_search, const std::vector<Directive>& to_check);
	
	int 	insert_directives(const std::vector<Directive>& to_insert);
	int		insert_locations(const std::list<Block>& to_insert);
};
std::ostream& operator<<(std::ostream& out,const server& to_print);
#endif