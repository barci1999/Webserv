/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ksudyn <ksudyn@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 18:45:47 by ksudyn            #+#    #+#             */
/*   Updated: 2026/04/06 15:30:58 by ksudyn           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIProcess.hpp"
#include "../Parseo_solo_toca_Pablo/server.hpp"

//////////////////////////////////////////////
// 🔹 UTILIDADES (cosas simples)
//////////////////////////////////////////////

/*
 * find_best_location(const std::string& path, const Block& server_config)
 * -----------------------------------------------------------------------
 * Busca la location más específica que coincide con la ruta de la request.
 *
 * Parámetros:
 * - path: ruta de la request (ej: "/cgi-bin/test.py")
 * - server_config: bloque server completo (contiene todas las locations)
 *
 * Cómo funciona:
 * 1. Recorre todas las locations definidas en el server.
 * 2. Comprueba si el path empieza por el nombre de la location.
 * 3. Se queda con la coincidencia más larga (longest match).
 * 4. Devuelve esa location.
 *
 * Relación con Webserv:
 * - Imita el comportamiento de nginx:
 *   location /cgi-bin/ → más específica que location /
 *
 * Ejemplo:
 *   path = "/cgi-bin/test.py"
 *   locations = ["/", "/cgi-bin/"]
 *   → devuelve "/cgi-bin/"
 *
 */
Block find_best_location(const std::string& path, const server& server_config)
{
	Block best;
	size_t max_len = 0;

	for (std::list<Block>::const_iterator it = server_config.get_srvLocations().begin();
		 it != server_config.get_srvLocations().end(); ++it)
	{
		std::string loc = it->getName();

		if (path.find(loc) == 0 && loc.length() > max_len)
		{
			best = *it;
			max_len = loc.length();
		}
	}
	return best;
}


//Guarda lo que hay despues del . en la extension | Saca la extensión de un archivo (.php, .py)
std::string CGIProcess::extractExtension(const std::string& path)
{
	size_t dotPos = path.find_last_of('.');

	if (dotPos == std::string::npos)
		return "";

	return path.substr(dotPos);
}

/*
 * extractCGIConfig(const Block& best_location, const Block& server_config)
 * ------------------------------------------------------------------------
 * Obtiene la configuración necesaria para ejecutar CGI:
 * - extensión del script (ej: .py, .php)
 * - ejecutable (ej: /usr/bin/python3)
 *
 * Parámetros:
 * - best_location: location seleccionada para esta request
 * - server_config: bloque server completo (fallback)
 *
 * Cómo funciona:
 * 1. Busca "cgi_extension" y "cgi_pass" en la location.
 * 2. Si no existen, busca en el server (fallback global).
 * 3. Guarda los valores en variables internas:
 *    - _cgiExtension
 *    - _cgiPass
 *
 * Relación con Webserv:
 * - Permite configuraciones como:
 *
 *   location /cgi-bin/ {
 *       cgi_extension .py;
 *       cgi_pass /usr/bin/python3;
 *   }
 *
 * - O configuración global en server.
 *
 * Importancia:
 * - Sin esto, no sabes:
 *   - qué archivos son CGI
 *   - cómo ejecutarlos
 *
 */

//Se busca cgi_extension y cgi_pass y se guarda en la variables, al principio las inicializo vacias
//Aqui buscamos dentro de cada location esas variables y si existen y no estan vacias, guardamos el primer argumento
//Lee del config (location) qué extensión y qué ejecutable CGI usar

void CGIProcess::extractCGIConfig(const Block& best_location, const server server_config)
{
	_cgiExtension.clear();
	_cgiPass.clear();

	Directive ext = Response::search_directive("cgi_extension", best_location);
	Directive pass = Response::search_directive("cgi_pass", best_location);

	// 🔁 fallback al server
	if (ext.name.empty())
		//Mensaje de erro de que no se encunentra

	if (pass.name.empty())
		//Mensaje de erro de que no se encunentra

	if (!ext.args.empty())
		_cgiExtension = ext.args[0];

	if (!pass.args.empty())
		_cgiPass = pass.args[0];
}


/*
 * isCGI(const Request& request, const Block& server_config)
 * ---------------------------------------------------------
 * Determina si la request debe ejecutarse como CGI.
 *
 * Parámetros:
 * - request: petición HTTP del cliente
 * - server_config: bloque server completo
 *
 * Cómo funciona:
 * 1. Obtiene el path de la request.
 * 2. Encuentra la mejor location (longest match).
 * 3. Extrae la configuración CGI (extensión + ejecutable).
 * 4. Extrae la extensión del archivo solicitado.
 * 5. Compara con la extensión CGI configurada.
 *
 * Resultado:
 * - true  → es CGI → hay que hacer fork + execve
 * - false → es archivo normal (GET)
 *
 * Ejemplo:
 *   request = "/cgi-bin/test.py"
 *   cgi_extension = ".py"
 *   → devuelve true
 *
 * Importancia:
 * - Decide el flujo principal del servidor:
 *   - CGI → proceso hijo
 *   - Static → make_Get()
 *
 */
// Aquí verifico que si el contenido del request es el mismo que la extension del location
//Comprueba si la request es CGI comparando extensión
bool CGIProcess::isCGI(const Request& request, const server& server_config)
{
	std::string path = request.get_path();

	Block best_location = find_best_location(path, server_config);

	if (best_location.getName().empty())
		return false;

	extractCGIConfig(best_location, server_config);

	if (_cgiExtension.empty() || _cgiPass.empty())
		return false;

	std::string extension = extractExtension(path);

	if (extension == _cgiExtension)
	{
		_scriptPath = path;
		return true;
	}
	return false;
}


//////////////////////////////////////////////
// 🔹 PATHS Y ARCHIVOS
//////////////////////////////////////////////

//CREAMOS EL FULLPATH
// Construye la ruta real del archivo en disco
/*
 * buildFullPath(const Request& request, const Block& server_config)
 * ------------------------------------------------------------------
 * Construye la ruta real en disco del recurso solicitado.
 *
 * Parámetros:
 * - request: petición HTTP
 * - server_config: configuración del server
 *
 * Cómo funciona:
 * 1. Encuentra la mejor location para el path.
 * 2. Obtiene el root:
 *    - primero intenta en location
 *    - si no existe → usa root del server
 * 3. Calcula el path relativo eliminando el prefijo de la location.
 * 4. Une root + relative path.
 *
 * Ejemplo:
 *   request: "/cgi-bin/test.py"
 *   location: "/cgi-bin/"
 *   root: "/var/www/cgi/"
 *
 *   → full_path = "/var/www/cgi/test.py"
 *
 * Importancia:
 * - Necesario para:
 *   - abrir archivos
 *   - ejecutar scripts CGI
 *   - evitar errores de ruta en execve()
 *
 */
std::string CGIProcess::buildFullPath(const Request& request, const server& server_config)
{
	std::string path = request.get_path();

	Block best_location = find_best_location(path, server_config);

	Directive root = Response::search_directive("root", best_location);
	if (root.name.empty())
		root = server_config.get_srvRoot();

	std::string root_path = root.args[0];

	size_t prefix_len = best_location.getName().length();
	std::string relative = path.substr(prefix_len);

	if (!relative.empty() && relative[0] == '/')
		relative.erase(0, 1);

	if (!root_path.empty() && root_path[root_path.size() - 1] != '/')
		root_path += '/';

	return root_path + relative;
}



//Si por algun casual el root ya termina en / no se añade uno.
//Al final se guarda _fullPath = buildFullPath(request, location); en la funcion que se llame.

// //Lee un archivo normal y devuelve su contenido
// std::string CGIProcess::serveStaticFile(const Request& request, const Block& location)
// {
// 	std::string fullPath = buildFullPath(request, location);//se construye la ruta real del archivo

// 	std::ifstream file(fullPath.c_str());

// 	if (!file.is_open())
// 		return "404 Not Found";

// 	std::stringstream buffer;
// 	buffer << file.rdbuf();//esto te devuelve todo el contenido del archivo y se copia con << dentro de buffer

// 	return buffer.str();
// }


//////////////////////////////////////////////
// 🔹 EJECUCION CGI (CORE)
//////////////////////////////////////////////

//Inicio de prueba de execute

//Lanza el proceso CGI (fork + pipes) | ESta es la vieja, la dejo por si hay que revisar algo 

//Nuevo execute, una version no bloqueante
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
    if (request.get_method() == "POST")
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
 *
 * Cómo funciona:
 * 1. Crea:
 *    - _inputPipe  → padre escribe → hijo lee (stdin)
 *    - _outputPipe → hijo escribe → padre lee (stdout)
 * 2. Configura ambos en modo NO BLOQUEANTE con fcntl().
 *
 * Importancia:
 * - Permite comunicación asíncrona sin bloquear el servidor.
 *
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
 *
 * Cómo funciona:
 * 1. Llama a fork().
 * 2. Guarda el PID:
 *    - 0 → proceso hijo
 *    - >0 → proceso padre
 *
 * Importancia:
 * - Permite ejecutar el CGI en un proceso separado.
 *
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
 *
 * Parámetros:
 * - request: petición HTTP del cliente
 *
 * Cómo funciona:
 * 1. Crea un vector<string> con variables CGI estándar:
 *    - REQUEST_METHOD
 *    - SERVER_PROTOCOL
 *    - QUERY_STRING
 *    ...
 * 2. Añade información del script:
 *    - SCRIPT_FILENAME → ruta absoluta
 *    - SCRIPT_NAME     → path original
 * 3. Añade headers importantes:
 *    - CONTENT_LENGTH
 *    - CONTENT_TYPE
 * 4. Añade variable especial:
 *    - REDIRECT_STATUS=200 (necesaria para PHP)
 * 5. Convierte vector<string> → char** (formato execve)
 *
 * Formato final:
 *   ["KEY=VALUE", "KEY=VALUE", ..., NULL]
 *
 * Importancia:
 * - El CGI depende completamente de estas variables.
 * - Sin ellas:
 *   - PHP no funciona ❌
 *   - POST no funciona ❌
 *   - QUERY_STRING vacío ❌
 *
 */
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


/*
 * setupChildProcess(const Request& request)
 * ------------------------------------------
 * Configura el proceso hijo antes de ejecutar el CGI.
 *
 * Parámetros:
 * - request: petición HTTP
 *
 * Cómo funciona:
 * 1. Redirige:
 *    - stdin  ← pipe de entrada (POST)
 *    - stdout → pipe de salida (respuesta CGI)
 *
 * 2. Cierra todos los pipes innecesarios.
 *
 * 3. Cambia el directorio de trabajo al del script:
 *    - IMPORTANTE para rutas relativas
 *
 * 4. Prepara argv:
 *    - argv[0] → ejecutable (ej: python3)
 *    - argv[1] → script
 *
 * 5. Construye variables de entorno con buildEnv()
 *
 * 6. Ejecuta execve():
 *    - reemplaza el proceso hijo por el CGI
 *
 * 7. Si falla:
 *    → imprime error y termina
 *
 * Relación con CGI:
 * - Esto es literalmente lo que hace Apache/nginx internamente.
 *
 * Importancia:
 * - Aquí ocurre la magia:
 *   fork + execve = CGI real
 *
 */
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

	// Es un mensaje para verificar un error
	std::cerr << "CGI cgiPass: [" << _cgiPass << "]" << std::endl;

    // 🔹 5. Ejecutar CGI
    execve(_cgiPass.c_str(), argv, env);

    // 🔴 Si llega aquí → error
    perror("execve failed");
    exit(1);
}

//////////////////////////////////////////////
// 🔹 POLL READY (LO NUEVO)
//////////////////////////////////////////////

/*
 * getFD() const
 * --------------
 * Devuelve el file descriptor del pipe de salida del CGI.
 *
 * Retorno:
 * - fd que debe ser monitorizado con poll()
 *
 * Importancia:
 * - Permite al servidor saber cuándo hay datos listos para leer.
 *
 */
int CGIProcess::getFD() const
{
    return _outputPipe[0];
}

/*
 * readFromPipe()
 * ----------------
 * Lee la salida del CGI de forma NO BLOQUEANTE.
 *
 * Cómo funciona:
 * 1. Intenta leer datos del pipe en un bucle.
 * 2. Si hay datos → los añade a _buffer.
 * 3. Si read() devuelve:
 *    - >0 → sigue leyendo
 *    - 0  → EOF → el CGI terminó → _finished = true
 *    - -1 → EAGAIN → no hay datos aún (NO error)
 *
 * Relación con poll():
 * - Se llama cuando poll() indica que hay datos disponibles.
 *
 * Importancia:
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
 *
 * Cómo funciona:
 * 1. Si _finished ya es true → retorna true.
 * 2. Llama a waitpid() con WNOHANG:
 *    - 0 → sigue ejecutándose
 *    - >0 → terminó
 * 3. Si terminó → marca _finished = true.
 *
 * Importancia:
 * - Permite saber cuándo construir la respuesta final.
 *
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

/*
 * buildResponse()
 * ----------------
 * Construye la Response HTTP a partir de la salida del CGI.
 *
 * Cómo funciona:
 * 1. Usa el buffer acumulado (_buffer).
 * 2. Llama a parseCGIResponse().
 * 3. Devuelve un objeto Response listo.
 *
 * Importancia:
 * - Convierte salida CGI → respuesta HTTP real.
 *
 */
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
 * Convierte la salida RAW de un CGI en una Response HTTP.
 *
 * Parámetros:
 * - output: string con toda la salida del CGI (headers + body)
 *
 * Cómo funciona:
 * 1. Busca la separación entre headers y body:
 *    "\r\n\r\n"
 * 2. Si no existe:
 *    → error → devuelve 500 (CGI mal formado)
 * 3. Divide:
 *    - headerPart → cabeceras CGI
 *    - bodyPart   → contenido
 * 4. Procesa cada línea de header:
 *    - Busca "key: value"
 *    - Limpia espacios y '\r'
 * 5. Caso especial:
 *    - "Status" → define código HTTP y reason phrase
 * 6. El resto de headers:
 *    → se añaden a la Response
 * 7. Asigna el body final
 *
 * Relación con CGI:
 * - Un CGI devuelve algo como:
 *
 *   Status: 200 OK\r\n
 *   Content-Type: text/html\r\n
 *   \r\n
 *   <html>...</html>
 *
 * Importancia:
 * - Traduce salida CGI → respuesta HTTP válida.
 * - Sin esto, el navegador no entiende nada.
 *
 */
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

	return response;
}

const std::string& CGIProcess::getBuffer() const
{
    return _buffer;
}





//Esta funcion decide que hacer en base a si es CGI o no
// std::string CGIProcess::handleRequest(Request& request, Block& location)
// {
// 	CGIProcess cgi;

// 	if (cgi.isCGI(request, location))
// 	{
// 		Response temp = execute(request, location);
// 		return res_to_str(temp);
// 	}
// 	else
// 	{
// 		Response temp(request,location);
// 		return res_to_str(temp);
// 	}
// }
// YA NO SE UTILIZA AQUI, LO QUE HACE SE DEBE HACER EN EL SERVER
