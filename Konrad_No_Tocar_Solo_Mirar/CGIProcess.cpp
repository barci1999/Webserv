/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 18:45:47 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/06 21:30:22 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIProcess.hpp"
#include "../Parseo_solo_toca_Pablo/server.hpp"
#include "../utils.hpp"
#include "../pollLoop.hpp"

//////////////////////////////////////////////
// 🔹 UTILIDADES (cosas simples)
//////////////////////////////////////////////

/*
 * extractCGIConfig(const Block& best_location, const server& server_config)
 * ------------------------------------------------------------------------
	Se busca cgi_extension y cgi_pass y se guarda en la variables, se usa el contenedor map para guardar datos
	Aqui buscamos dentro de cada location esas variables y si existen y no estan vacias,
	guardamos los argumentos usando el for recorriendo los argumentos que hay
*/
void CGIProcess::extractCGIConfig(const Block& best_location, const server& server_config)
{
	_cgimap.clear();
	(void)server_config;
	Directive ext = search_directive("cgi_extension", best_location);
	Directive pass = search_directive("cgi_pass", best_location);

	if (ext.args.empty() || pass.args.empty())
		return;

	if (ext.args.size() != pass.args.size())
	{
		std::cerr << "CGI config mismatch: extensions and passes size differ" << std::endl;
		return;
	}

	for (size_t i = 0; i < ext.args.size(); i++)
	{
		std::string extension = ext.args[i];
		std::string route = pass.args[i];

		if (!extension.empty() && extension[0] == '.')
			_cgimap[extension] = route;
	}
}


/*
 * isCGI(const Request& request, const Block& server_config)
 * ---------------------------------------------------------
 	Determina si la request debe ejecutarse como CGI.
 	Aquí buscamos la extensión y obtenemos el ejecutable correspondiente.
	Además, guardamos la ruta del script para poder construir la ruta completa
	y ejecutarlo correctamente.
 */
bool CGIProcess::isCGI(const Request& request, const server& server_config)
{
	std::string path = request.get_path();

	Block best_location = find_best_location(path, server_config);

	if (best_location.getName().empty())
		return false;

	extractCGIConfig(best_location, server_config);

	std::string extension = extractExtension(path);

	std::map<std::string, std::string>::iterator it = _cgimap.find(extension);
	if (it != _cgimap.end())
	{
		_scriptPath = path;
		_cgiPass = it->second;
		return true;
	}
	return false;
}
// El sistema asume que el archivo de configuración es coherente.
// Si el orden entre cgi_extension y cgi_pass no coincide,
// el comportamiento será incorrecto, ya que el emparejamiento se hace por posición.
// Si nos pasan .py .php y en la extension usr/bin/php usr/bin/python no funcionaria.



//////////////////////////////////////////////
// 🔹 PATHS Y ARCHIVOS
//////////////////////////////////////////////

//CREAMOS EL FULLPATH
/*
 * buildFullPath(const Request& request, const server& server_config)
 * ------------------------------------------------------------------
 * Construye la ruta absoluta del script.
 * Usa root de la location o del server.
 * Elimina el prefijo de la location y concatena.
 */
std::string CGIProcess::buildFullPath(const Request& request, const server& server_config)
{
	std::string path = request.get_path();

	Block best_location = find_best_location(path, server_config);

	Directive root = search_directive("root", best_location);
	if (root.name.empty())
		root = server_config.get_srvRoot();

	std::string root_path = root.args[0];

	size_t prefix_len = best_location.getName().length();
	std::string relative = path.substr(prefix_len);

	if (!relative.empty() && relative[0] == '/')
		relative.erase(0, 1);

	if (!root_path.empty() && root_path[root_path.size() - 1] != '/')
		root_path += '/';

	std::string scriptPath = root_path + relative;
	std::string temp ="/";
	if (!scriptPath.empty() && scriptPath[0] !='/')
	{
		scriptPath = temp + scriptPath;
	}

	return scriptPath;
}


//////////////////////////////////////////////
// 🔹 EJECUCION CGI (CORE)
//////////////////////////////////////////////

/*
 * execute(const Request& request, const server& server_config)
 * ------------------------------------------------------------
 * Lanza la ejecución de un CGI de forma NO BLOQUEANTE.
 * Lanza el CGI de forma no bloqueante.
 * Crea pipes, hace fork y ejecuta el hijo.
 * El padre solo escribe (POST) y continúa.
 */
void CGIProcess::execute(const Request& request, const server& server_config, std::vector<pollfd>& pollFds)
{
    _fullPath = buildFullPath(request, server_config);
    _finished = false;
    _buffer.clear();
    _byteSent = 0; 

    createPipes();
    forkProcess();

    if (_pid == 0)
    {
        for (std::vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it)
            close(it->fd);
        
        setupChildProcess(request);
        exit(1);
    }

    close(_inputPipe[0]);  
    close(_outputPipe[1]); 

    pollfd pfd_out;
    pfd_out.fd = _outputPipe[0];
    pfd_out.events = POLLIN;
	pfd_out.revents = 0;
    pollFds.push_back(pfd_out);

    if (request.get_method() == "POST")
    {
        _bodyToSend = request.get_body();
        
        pollfd pfd_in;
        pfd_in.fd = _inputPipe[1];
        pfd_in.events = POLLOUT; 
		pfd_in.revents = 0;
        pollFds.push_back(pfd_in);

    }
    else
    {

        close(_inputPipe[1]);
        _inputPipe[1] = -1;
    }
}
void CGIProcess::writeToPipe()
{
    std::cout << "DEBUG: Intentando escribir. Body size: " << _bodyToSend.size() 
              << " | Bytes sent: " << _byteSent << std::endl;

    if (_inputPipe[1] == -1 || _bodyToSend.empty()) {
        std::cout << "DEBUG: Abortando write: Pipe cerrado o Body vacío" << std::endl;
        return;
    }

    ssize_t bytes = write(_inputPipe[1], _bodyToSend.c_str() + _byteSent, 
                          _bodyToSend.size() - _byteSent);
    
    if (bytes > 0) {
        _byteSent += bytes;
        std::cout << "DEBUG: Escritos " << bytes << " bytes al CGI" << std::endl;
    }

    if (_byteSent >= _bodyToSend.size()) {
        std::cout << "DEBUG: Finalizado. Cerrando pipe de entrada del CGI." << std::endl;
        close(_inputPipe[1]);
        _inputPipe[1] = -1; 
    }
}


/*
 * createPipes()
 * --------------
 * Crea los pipes de comunicación entre proceso padre e hijo.
 * Configura lectura/escritura en modo no bloqueante.
 */
void CGIProcess::createPipes()
{
	if (pipe(_inputPipe) < 0)
		throw std::runtime_error("pipe failed");

	if (pipe(_outputPipe) < 0)
		throw std::runtime_error("pipe failed");
	
    int flags = fcntl(_outputPipe[0], F_GETFL, 0);
    fcntl(_outputPipe[0], F_SETFL, flags | O_NONBLOCK);

    flags = fcntl(_inputPipe[1], F_GETFL, 0);
    fcntl(_inputPipe[1], F_SETFL, flags | O_NONBLOCK);
	//Esto lee del CGI y escribe en el CGI de forma no bloqueante usando el fcntl.
	// DE esta manera se hace una vez, afecta a todos los usos.
}

/*
 * forkProcess()
 * --------------
 * Duplica el proceso en padre e hijo.
 * Hace fork y guarda el PID.
 */
void CGIProcess::forkProcess()
{
	_pid = fork();

	if (_pid < 0)
		throw std::runtime_error("fork failed");
}


/*
 * buildEnv(const Request& request)
 * --------------------------------
 * Construye las variables de entorno necesarias para ejecutar el CGI.
 * Incluye método, path, headers y datos necesarios para PHP.
 * 
 * Importancia:
 * - El CGI depende completamente de estas variables.
 * - Sin ellas:
 *   - PHP no funciona ❌
 *   - POST no funciona ❌
 *   - QUERY_STRING vacío ❌
 */
char **CGIProcess::buildEnv(const Request& request)
{
    std::vector<std::string> env;

    // 📜 CGI básico obligatorio
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("REQUEST_METHOD=" + request.get_method());
    env.push_back("SERVER_PROTOCOL=" + request.get_version());

    // 📍 Script info
    env.push_back("SCRIPT_FILENAME=" + _fullPath);
    env.push_back("SCRIPT_NAME=" + request.get_path());
    env.push_back("PATH_INFO=" + request.get_path());

    // 📍 Query string (GET)
    env.push_back("QUERY_STRING=" + request.get_query());

    // 📍 Manejo de POST (Content-Length y Content-Type)
    if (request.get_method() == "POST")
    {
        // Convertimos el tamaño del body a string usando stringstream (C++98)
        std::stringstream ss;
        ss << request.get_body().size();
        env.push_back("CONTENT_LENGTH=" + ss.str());

        // Buscamos el Content-Type (siendo un poco flexibles con las mayúsculas)
        const std::map<std::string, std::string>& headers = request.get_headers();
        if (headers.count("Content-Type"))
            env.push_back("CONTENT_TYPE=" + headers.at("Content-Type"));
        else if (headers.count("content-type"))
            env.push_back("CONTENT_TYPE=" + headers.at("content-type"));
        else
            env.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
    }
    else 
    {
        // Para GET, el estándar suele pedir que Content-Length sea 0 o no esté
        env.push_back("CONTENT_LENGTH=0");
    }

    // Importante para PHP
    env.push_back("REDIRECT_STATUS=200");

    // 📍 Convertir a char** para execve
    char **envp = new char*[env.size() + 1];

    for (size_t i = 0; i < env.size(); i++)
        envp[i] = strdup(env[i].c_str());

    envp[env.size()] = NULL;

    return envp;
}
bool CGIProcess::isBodyfinished()
{
	return _byteSent >=_bodyToSend.size();
}
/*
 * setupChildProcess(const Request& request)
 * ------------------------------------------
 * Configura el proceso hijo antes de ejecutar el CGI.
 * Configura el hijo:
 * redirige stdin/stdout, hace chdir y ejecuta execve.
 */
void CGIProcess::setupChildProcess(const Request& request)
{
    dup2(_inputPipe[0], STDIN_FILENO);

    dup2(_outputPipe[1], STDOUT_FILENO);

    close(_inputPipe[0]);
    close(_inputPipe[1]);
    close(_outputPipe[0]);
    close(_outputPipe[1]);

	std::string dir = _fullPath.substr(0, _fullPath.find_last_of('/'));
	if (!dir.empty() && dir[0] == '/')
	{
		dir.erase(0,1);
	}

	if (chdir(dir.c_str()) != 0)
	{
		perror("chdir failed");
		exit(1);
	}
    std::string scriptName = _fullPath.substr(_fullPath.find_last_of('/') + 1);
    char *argv[3];

    argv[0] = strdup(("/" + _cgiPass).c_str());  // "/usr/bin/python3"
    argv[1] = strdup(scriptName.c_str());        // "test.py"
    argv[2] = NULL;

    char **env = buildEnv(request);
    execve(argv[0], argv, env);

    perror("execve failed");
    exit(127);
}


//////////////////////////////////////////////
// 🔹 POLL READY (LO NUEVO)
//////////////////////////////////////////////

/*
 * getFD() const
 * Devuelve el fd de salida del CGI (para poll).
 */
int CGIProcess::getFD() const
{
    return _outputPipe[0];
}

/*
 * readFromPipe()
 * ----------------
 * Lee la salida del CGI de forma NO BLOQUEANTE.
 * Guarda datos en _buffer.
 * EOF → marca como terminado.
 * - Permite leer progresivamente sin bloquear el servidor.
 *
 */
void CGIProcess::readFromPipe()
{
    char buffer[4096];
    ssize_t bytes;

    // Lee todo lo que haya en el pipe en este momento
    while ((bytes = read(_outputPipe[0], buffer, sizeof(buffer))) > 0)
    {
        _buffer.append(buffer, bytes);
    }
    
    // Si bytes es 0, es un EOF real (el pipe se cerró)
    if (bytes == 0)
        _finished = true;
        
    // Si es -1 y errno es EWOULDBLOCK, simplemente no hay más datos por ahora.
    // Salimos y el pollLoop volverá a llamarnos cuando el CGI escriba más.
}


/*
 * isFinished()
 * --------------
 * Comprueba si el CGI ha terminado.
 * Usa waitpid no bloqueante.
 */
bool CGIProcess::isFinished()
{
	if (_finished)
		return true;
	
	int status;
	
	pid_t result = waitpid(_pid, &status, WNOHANG);

	if(result == 0)
		return false;
	
	_finished = true;
	return true;
}

// Es solo encapsulamiento para no llamar al _buffer y antes habia mas cosas y ya la dejo así
Response CGIProcess::buildResponse()
{
	return parseCGIResponse(_buffer);
}


//////////////////////////////////////////////
// 🔹 PARSEO CGI
//////////////////////////////////////////////


/*
 * parseCGIResponse(const std::string& output)
 * --------------------------------------------
 * Parsea la salida del CGI (headers + body).
 * Convierte a Response HTTP válida.
 */
Response CGIProcess::parseCGIResponse(const std::string& output)
{
	Response response;

	size_t pos = output.find("\r\n\r\n");
	size_t separator_len = 4;

	if (pos == std::string::npos)
	{
		pos = output.find("\n\n");
		separator_len = 2;
	}
	std::cout<<output<<std::endl;
	if (pos == std::string::npos)
	{
		std::cout<<"GGGGGGGGGGGGG"<<std::endl;
		response.set_statuscode(500);
		response.set_reasonphrase("Internal Server Error");
		response.set_body("CGI malformed response");
		return response;
	}

	std::string headerPart = output.substr(0, pos);
	std::string bodyPart = output.substr(pos + separator_len);
	
	std::istringstream stream(headerPart);
	std::string line;

	while (std::getline(stream, line))
	{
		if (line.empty() || line == "\r")
			continue;

		size_t sep = line.find(':');

		if (sep == std::string::npos)
			continue;

		std::string key = line.substr(0, sep);
		std::string value = line.substr(sep + 1);

		// limpiar espacio inicial
		if (!value.empty() && value[0] == ' ')
			value.erase(0, 1);

		// limpiar \r
		if (!value.empty() && value[value.size() - 1] == '\r')
			value.erase(value.size() - 1);

		// Manejo de Status
        if (key == "Status")
        {
            size_t spacePos = value.find(' ');
            if (spacePos != std::string::npos)
            {
				response.set_statuscode(std::atoi(value.substr(0, spacePos).c_str()));
                response.set_reasonphrase(value.substr(spacePos + 1));
            }
            else
            {
                response.set_statuscode(std::atoi(value.c_str()));
                response.set_reasonphrase("");
            }
        }
        else
        {
            response.addback_headers(key, value);
        }
	}

	response.set_body(bodyPart);

	// AQUÍ SE GENRAL EL HTTP
	response.set_version("HTTP/1.1");
	//response.set_statuscode(200);

	response.addback_headers("Content-Length", toString(bodyPart.size()));
	response.addback_headers("Connection", "close");

	return response;
}
