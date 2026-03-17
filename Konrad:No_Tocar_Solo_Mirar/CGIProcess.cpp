/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 18:45:47 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/17 17:32:33 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIProcess.hpp"
#include "server.hpp"

//Guarda lo que hay despues del . en la extension
std::string CGIProcess::extractExtension(const std::string& path)
{
	size_t dotPos = path.find_last_of('.');

	if (dotPos == std::string::npos)
		return "";

	return path.substr(dotPos);
}

//Se busca cgi_extension y cgi_pass y se guarda en la variables, al principio las inicializo vacias
//Aqui buscamos dentro de cada location esas variables y si existen y no estan vacias, guardamos el primer argumento
void CGIProcess::extractCGIConfig(const Block& location)
{
	const std::vector<Directive>& vect_directie = location.getDirectives();

	_cgiExtension = "";
	_cgiPass = "";

	for (size_t i = 0; i < vect_directie.size(); i++)
	{
		if (vect_directie[i].name == "cgi_extension" && !vect_directie[i].args.empty())
			_cgiExtension = vect_directie[i].args[0];

		if (vect_directie[i].name == "cgi_pass" && !vect_directie[i].args.empty())
			_cgiPass = vect_directie[i].args[0];
	}
}

// Aquí verifico que si el contenido del request es el mismo que la extension del location
bool CGIProcess::isCGI(const Request& request, const Block& location)
{
	extractCGIConfig(location);

	if (_cgiExtension.empty() || _cgiPass.empty())
		return false;

	std::string extension = extractExtension(request.get_path());

	if (extension == _cgiExtension)
	{
		_scriptPath = request.get_path();
		return true;
	}

	return false;
}

//CREAMOS EL FULLPATH

std::string CGIProcess::buildFullPath(const Request& request, const Block& location)
{

	Directive root = server::check_directives("root",location);
	std::string locationPrefix = location.getName(); 
	std::string relativePath = request.get_path().substr(locationPrefix.length());

	std::string rootPath = root.args[0];
	
	if (rootPath[rootPath.size() - 1] == '/')
		return rootPath + relativePath;

	return rootPath + "/" + relativePath;
}
//Si por algun casual el root ya termina en / no se añade uno.
//Al final se guarda _fullPath = buildFullPath(request, location); en la funcion que se llame.


std::string CGIProcess::serveStaticFile(const Request& request, const Block& location)
{
	std::string fullPath = buildFullPath(request, location);//se construye la ruta real del archivo

	std::ifstream file(fullPath.c_str());

	if (!file.is_open())
		return "404 Not Found";

	std::stringstream buffer;
	buffer << file.rdbuf();//esto te devuelve todo el contenido del archivo y se copia con << dentro de buffer

	return buffer.str();
}

std::string CGIProcess::execute(const Request& request, const Block& location)
{
	_fullPath = buildFullPath(request, location);

	std::ifstream file(_fullPath.c_str());

	if (!file.is_open())
		return "404 CGI script not found";

	return "CGI execution placeholder";
}

//Esta funcion decide que hacer en base a si es CGI o no
std::string CGIProcess::handleRequest(Request& request, Block& location)
{
	CGIProcess cgi;

	if (cgi.isCGI(request, location))
	{
		return execute(request, location);
	}
	else
	{
		return serveStaticFile(request, location);
	}
}

//Inicio de priueba de execute

std::string CGIProcess::execute(const Request& request, const Block& location)
{
	_fullPath = buildFullPath(request, location);

	createPipes();

	forkProcess();

	if (_pid == 0)
		setupChildProcess(request);
	else
		return handleParentProcess(request);

	return "";
}

void CGIProcess::createPipes()
{
	if (pipe(_inputPipe) < 0)
		throw std::runtime_error("pipe failed");

	if (pipe(_uotputPippe) < 0)
		throw std::runtime_error("pipe failed");
}

void CGIProcess::forkProcess()
{
	_pid = fork();

	if (_pid < 0)
		throw std::runtime_error("fork failed");
}

// char **CGIProces::buildEnv(const Request& request)
// {
// 	std::vector<std::string> env;

//     env.push_back("REQUEST_METHOD=" + request.method);
//     env.push_back("QUERY_STRING=" + request.query);
//     env.push_back("SCRIPT_FILENAME=" + _fullPath);

//     if (request.headers.count("Content-Length"))
//         env.push_back("CONTENT_LENGTH=" + request.headers.at("Content-Length"));

//     char **envp = new char*[env.size() + 1];

//     for (size_t i = 0; i < env.size(); i++)
//         envp[i] = strdup(env[i].c_str());

//     envp[env.size()] = NULL;

//     return envp;
// }

void CGIProcess::setupChildProcess(const Request& request)
{
	dup2(_inputPipe[0], STDIN_FILENO);
	dup2(_uotputPippe[1], STDOUT_FILENO);

	close(_inputPipe[1]);
	close(_uotputPippe[0]);

	char *argv[3];

	argv[0] = const_cast<char*>(_cgiPass.c_str());
	argv[1] = const_cast<char*>(_fullPath.c_str());
	argv[3] = NULL;

	//char **env = buildEnv(request);
	
}