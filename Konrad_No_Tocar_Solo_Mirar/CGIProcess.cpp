/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 18:45:47 by ksudyn            #+#    #+#             */
/*   Updated: 2026/03/30 21:08:01 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIProcess.hpp"
#include "../Parseo_solo_toca_Pablo/server.hpp"

//////////////////////////////////////////////
// 🔹 UTILIDADES (cosas simples)
//////////////////////////////////////////////

//Guarda lo que hay despues del . en la extension | Saca la extensión de un archivo (.php, .py)
std::string CGIProcess::extractExtension(const std::string& path)
{
	size_t dotPos = path.find_last_of('.');

	if (dotPos == std::string::npos)
		return "";

	return path.substr(dotPos);
}

//Se busca cgi_extension y cgi_pass y se guarda en la variables, al principio las inicializo vacias
//Aqui buscamos dentro de cada location esas variables y si existen y no estan vacias, guardamos el primer argumento
//Lee del config (location) qué extensión y qué ejecutable CGI usar
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
//Comprueba si la request es CGI comparando extensión
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


//////////////////////////////////////////////
// 🔹 PATHS Y ARCHIVOS
//////////////////////////////////////////////


//CREAMOS EL FULLPATH
// Construye la ruta real del archivo en disco
std::string CGIProcess::buildFullPath(const Request& request, const Block& location)
{

	Directive root = server::check_directives("root",location);
	std::string locationPrefix = location.getName(); 
	std::string relativePath = request.get_path().substr(locationPrefix.length());

	std::string rootPath = root.args[0];
	
	if (!rootPath.empty() && rootPath[rootPath.size() - 1] == '/')
		return rootPath + relativePath;

	return rootPath + "/" + relativePath;
}
//Si por algun casual el root ya termina en / no se añade uno.
//Al final se guarda _fullPath = buildFullPath(request, location); en la funcion que se llame.

//Lee un archivo normal y devuelve su contenido
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


//////////////////////////////////////////////
// 🔹 EJECUCION CGI (CORE)
//////////////////////////////////////////////


//Esta funcion decide que hacer en base a si es CGI o no
// std::string CGIProcess::handleRequest(Request& request, Block& location)
// {
// 	CGIProcess cgi;

// 	if (cgi.isCGI(request, location))
// 	{
// 		return execute(request, location);
// 	}
// 	else
// 	{
// 		return serveStaticFile(request, location);
// 	}
// }
// YA NO SE UTILIZA AQUI, LO QUE HACE SE DEBE HACER EN EL SERVER

//Inicio de priueba de execute

//Lanza el proceso CGI (fork + pipes) | ESta es la vieja, la dejo por si hay que revisar algo 
// std::string CGIProcess::execute(const Request& request, const Block& location)
// {
// 	_fullPath = buildFullPath(request, location);

// 	createPipes();

// 	forkProcess();

// 	if (_pid == 0)
// 	{
// 		setupChildProcess(request);
// 		exit(1);
// 	}
// 	else
// 		return handleParentProcess(request);
// }

//Nuevo execute, una version no bloqueante
void CGIProcess::execute(const Request& request, const Block& location)
{
    _fullPath = buildFullPath(request, location);
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
    if (request.get_method() == "POST")
    {
        const std::string& body = request.get_body();
        write(_inputPipe[1], body.c_str(), body.size());
    }

    close(_inputPipe[1]);
}

//Crea canales de comunicación entre padre e hijo
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

//Duplica el proceso (padre e hijo)
void CGIProcess::forkProcess()
{
	_pid = fork();

	if (_pid < 0)
		throw std::runtime_error("fork failed");
}

//Construye variables de entorno para el CGI (muy importante para PHP)
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

    // 🔥 Muy importante para PHP
    env.push_back("REDIRECT_STATUS=200");

    // Convertir a char**
    char **envp = new char*[env.size() + 1];

    for (size_t i = 0; i < env.size(); i++)
        envp[i] = strdup(env[i].c_str());

    envp[env.size()] = NULL;

    return envp;
}

//Configura el hijo: redirige stdin/stdout y ejecuta el CGI
void CGIProcess::setupChildProcess(const Request& request)
{
    // 🔹 1. Redirigir stdin y stdout
    dup2(_inputPipe[0], STDIN_FILENO);
    dup2(_outputPipe[1], STDOUT_FILENO);

    // 🔹 2. Cerrar pipes innecesarios
    close(_inputPipe[1]);
    close(_outputPipe[0]);
    close(_inputPipe[0]);
    close(_outputPipe[1]);


	// Extrae el directorio del script:
	// Cambia el directorio de ejecución del proceso hijo
	std::string dir = _fullPath.substr(0, _fullPath.find_last_of('/'));
	if (chdir(dir.c_str()) != 0)
	{
		perror("chdir failed");
		exit(1);
	}
	// Si NO haces chdir():
	// Buscaría en el directorio donde lanzaste el server ❌
	// Fallaría aunque el archivo exista ❌
	
	
    // 🔹 3. Preparar argv
    char *argv[3];

    argv[0] = const_cast<char*>(_cgiPass.c_str());   // ej: /usr/bin/python3
    argv[1] = const_cast<char*>(_fullPath.c_str());  // script.py
    argv[2] = NULL;

    // 🔹 4. Crear entorno
    char **env = buildEnv(request);

    // 🔹 5. Ejecutar CGI
    execve(_cgiPass.c_str(), argv, env);

    // 🔴 Si llega aquí → error
    perror("execve failed");
    exit(1);
}

//Configura el hijo: redirige stdin/stdout y ejecuta el CGI
// std::string CGIProcess::handleParentProcess(const Request& request)
// {
//     // 🔹 1. Cerrar extremos que no usamos
//     close(_inputPipe[0]);     // el padre no lee de inputPipe
//     close(_outputPipe[1]);   // el padre no escribe en outputPipe

//     // 🔹 2. Si es POST → enviar body al CGI
//     if (request.get_method() == "POST")
//     {
//         const std::string& body = request.get_body();

//         write(_inputPipe[1], body.c_str(), body.size());
//     }

//     // 🔹 IMPORTANTE: cerrar después de escribir
//     close(_inputPipe[1]);

//     // 🔹 3. Leer salida del CGI
//     std::string output;
//     char buffer[4096];
//     ssize_t bytes;

//     while ((bytes = read(_outputPipe[0], buffer, sizeof(buffer))) > 0)//ESTO ES BLOQEUANTE, AL FINCLA SE QUITARA
//     {
//         output.append(buffer, bytes);
//     }

// 	//ESTO ES NO BLOQUENATE Y SERA ASI AL FINAL
// 	// poner pipe en no bloqueante
// 	// fcntl(_outputPipe[0], F_SETFL, O_NONBLOCK);

// 	// ssize_t bytes = read(_outputPipe[0], buffer, sizeof(buffer));

// 	// if (bytes > 0)
// 	// 	output.append(buffer, bytes);

// 	// else if (bytes == -1 && errno == EAGAIN)
// 	// {
// 	// 	// 🔥 no hay datos aún → volver al poll()
// 	// }
		
//     close(_outputPipe[0]);

//     // 🔹 4. Esperar al hijo
//     int status;
//     waitpid(_pid, &status, 0);

//     // 🔹 5. Devolver output (luego lo parsearás a HTTP)
//     Response res = parseCGIResponse(output);
// 	return res.get_body(); // temporal
// }
//En principio ya no se utiliza, el nuevo execute hace su trabajo de forma no bloqueante



//////////////////////////////////////////////
// 🔹 POLL READY (LO NUEVO)
//////////////////////////////////////////////

int CGIProcess::getFD() const
{
    return _outputPipe[0];
}

// Leer pipe sin bloquear
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


// Saber si terminó
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

// Parsear output CGI a Response
Response CGIProcess::buildResponse()
{
	return parseCGIResponse(_buffer);
}


//////////////////////////////////////////////
// 🔹 PARSEO CGI
//////////////////////////////////////////////

//Convierte la salida del CGI en una Response HTTP
Response CGIProcess::parseCGIResponse(const std::string& output)
{
	Response response;

	size_t pos = output.find("\r\n\r\n");

	if (pos == std::string::npos)
	{
		response.set_statuscode(500);
		response.set_reasonphrase("Internal Server Error");
		response.set_body("CGI malformed response");
		return response;
	}

	std::string headerPart = output.substr(0, pos);
	std::string bodyPart = output.substr(pos + 4);

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
                response.set_statuscode(std::stoi(value.substr(0, spacePos)));
                response.set_reasonphrase(value.substr(spacePos + 1));
            }
            else
            {
                response.set_statuscode(std::stoi(value));
                response.set_reasonphrase("");
            }
        }
        else
        {
            response.addback_headers(key, value);
        }
	}

	response.set_body(bodyPart);

	return response;
}
