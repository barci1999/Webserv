/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablalva <pablalva@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 18:45:47 by ksudyn            #+#    #+#             */
/*   Updated: 2026/05/04 14:15:18 by pablalva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIProcess.hpp"
#include "../Parseo_solo_toca_Pablo/server.hpp"
#include "../utils.hpp"

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
		std::cerr << "CGI EXT: " << extension << std::endl;//DEBUS que puse para la parte bonus
		std::cerr << "CGI PAS: " << _cgiPass << std::endl;
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
	
	// 🔥 DEBUG AQUÍ
	// std::cout << "PATH: " << path << std::endl;
	// std::cout << "RELATIVE: " << relative << std::endl;
	// std::cout << "ROOT: " << root_path << std::endl;
	// std::cout << "SCRIPT PATH: " << scriptPath << std::endl;

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
void CGIProcess::execute(const Request& request, const server& server_config)
{
    _fullPath = buildFullPath(request, server_config);
    _finished = false;
    _buffer.clear();

    createPipes();
    forkProcess();

    if (_pid == 0)
    {
        setupChildProcess(request);
        exit(1);
    }

    close(_inputPipe[0]);
    close(_outputPipe[1]);

	//Una vez que se haga no bloqueante esto desde aqui hasta el final se quitara de esta funcion.
    if (request.get_method() == "POST")// TODO: Por que body? En GET no hay body
    {
        const std::string& body = request.get_body();
        write(_inputPipe[1], body.c_str(), body.size());
    }	

    close(_inputPipe[1]);
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
	
	// No se si poner los extremos de lectura en non-blocking es aqui, luego se vera.
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

    // 📍 Headers importantes
    const std::map<std::string, std::string>& headers = request.get_headers();

    if (headers.count("Content-Length"))
        env.push_back("CONTENT_LENGTH=" + headers.at("Content-Length"));

    if (headers.count("Content-Type"))
        env.push_back("CONTENT_TYPE=" + headers.at("Content-Type"));

    // Importante para PHP
    env.push_back("REDIRECT_STATUS=200");

    // Convertir a char**
    char **envp = new char*[env.size() + 1];

    for (size_t i = 0; i < env.size(); i++)
        envp[i] = strdup(env[i].c_str());

    envp[env.size()] = NULL;

    return envp;
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
    // 🔹 1. Redirigir stdin (pipe entrada) → STDIN del CGI
    dup2(_inputPipe[0], 0);

    // 🔹 2. Redirigir stdout (pipe salida) → STDOUT del CGI
    dup2(_outputPipe[1], 1);

    // 🔹 3. Cerrar todos los pipes (ya no se necesitan tras dup2)
    close(_inputPipe[0]);
    close(_inputPipe[1]);
    close(_outputPipe[0]);
    close(_outputPipe[1]);

	// 🔹 4. Obtener el directorio del script (SIN el nombre del archivo)
    // Ej: "sgoinfre/.../cgi-bin/test.py" → "sgoinfre/.../cgi-bin"
	std::string dir = _fullPath.substr(0, _fullPath.find_last_of('/'));
	if (!dir.empty() && dir[0] == '/')
	{
		dir.erase(0,1);
	}

	// 🔹 5. Crear el directorio
	if (chdir(dir.c_str()) != 0)
	{
		perror("chdir failed");
		exit(1);
	}
	// Si NO haces chdir():
	// Buscaría en el directorio donde se lanzo el server
	// Fallaría aunque el archivo exista
	
	
	// 🔹 6. Obtener SOLO el nombre del script
    // Ej: "sgoinfre/.../cgi-bin/test.py" → "test.py"
    std::string scriptName = _fullPath.substr(_fullPath.find_last_of('/') + 1);
	
    // 🔹 7. Preparar argumentos para execve
    char *argv[3];

	//SE AÑADE / AL PRINCIIO DE _cgiPass por que si no no funciona
    argv[0] = strdup(("/" + _cgiPass).c_str());  // "/usr/bin/python3"
    argv[1] = strdup(scriptName.c_str());        // "test.py"
    argv[2] = NULL;

    // 🔹 8. Construir entorno CGI
    char **env = buildEnv(request);

	//DESDE AQUI
	// Es un mensaje para verificar un error. Aqui no hay / al principio
	std::cerr << "CGI cgiPass: [" << _cgiPass << "]" << std::endl;
	//DEBUG TRAS AÑADIR / EN EL ARGV[0]
	std::cerr << "CGI cgiPass2: [" << argv[0] << "]" << std::endl;
	// 🔹 DEBUG útil por que no entiendo
	std::cerr << "EXECVE PATH: " << argv[0] << std::endl;
    std::cerr << "SCRIPT: " << argv[1] << std::endl;
    std::cerr << "CWD: " << dir << std::endl;
	std::cerr << "DEBUG: ejecutando execve" << std::endl;
	//HASTA AQUI SON DEBUGS QUE SE PUEDEN QUITAR AL FINAL

	// 🔹 9. Ejecutar CGI
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

	while ((bytes = read(_outputPipe[0], buffer, sizeof(buffer))) > 0)
	{
		_buffer.append(buffer, bytes);
	}
	
	if( bytes == 0)
	{
		// EOF -> CGI terminó de escribir | No se hace nada con bytes < 0 por que EOF = 0 es que termino
		// y -1 puede ser EAGAIN por lo que hay que ignoorar
		_finished = true;
		close(_outputPipe[0]);
	}
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

	if (pos == std::string::npos)
	{
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
	response.set_statuscode(200);

	response.addback_headers("Content-Length", toString(bodyPart.size()));
	response.addback_headers("Connection", "close");

	return response;
}
