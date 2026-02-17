# README – Módulo CGI (webserv)

## 🎯 Objetivo

Implementar el soporte CGI en nuestro servidor web conforme al estándar CGI/1.1, permitiendo ejecutar scripts externos (por ejemplo .py, .php) y devolver su salida como respuesta HTTP válida.

Este módulo será responsable de:

- Detectar cuándo una petición es CGI.
- Ejecutar el script correspondiente.
- Pasar la información correcta al script.
- Leer su salida.
- Convertir esa salida en una respuesta HTTP válida.

---

## 🧠 ¿Qué es CGI en nuestro proyecto?

CGI (Common Gateway Interface) es el mecanismo que permite que el servidor ejecute un programa externo para generar una respuesta dinámica.

En vez de enviar un archivo estático directamente, el servidor:

1. Ejecuta el script como proceso separado.
2. Le pasa información sobre la petición.
3. Recibe lo que el script imprime.
4. Lo envía al cliente como respuesta HTTP.

---

## 📌 ¿Cuándo se usa CGI?

Cuando la ruta solicitada corresponde a una extensión configurada como CGI en el archivo de configuración.

Ejemplo:

/index.html  → archivo estático  
/test.py     → CGI

Necesitamos que el parser del config nos proporcione:

- Las extensiones CGI soportadas.
- El ejecutable asociado a cada extensión.

Ejemplo esperado:

".py"  → "/usr/bin/python3"  
".php" → "/usr/bin/php-cgi"

---

## 📦 Qué necesita recibir el módulo CGI

Para poder funcionar correctamente, el módulo CGI necesita recibir:

### 1️⃣ Un objeto Request completamente parseado

Debe contener como mínimo:

- method (GET, POST, DELETE…)
- path (ruta solicitada)
- query_string (lo que viene después de “?”)
- headers (map completo)
- body (si es POST)

Ejemplo esperado:

class Request
{
    std::string method;
    std::string path;
    std::string query;
    std::map<std::string, std::string> headers;
    std::string body;
};

---

### 2️⃣ Información del config

Necesitamos acceso a:

- Mapa de extensiones CGI
- Root del server/location
- Puerto
- Server name

---

## ⚙️ Responsabilidades del módulo CGI

El módulo CGI deberá:

### 1️⃣ Construir variables de entorno (según CGI/1.1)

Ejemplo:

REQUEST_METHOD=GET  
QUERY_STRING=name=juan  
CONTENT_LENGTH=123  
CONTENT_TYPE=application/x-www-form-urlencoded  
SCRIPT_NAME=/test.py  
SERVER_PROTOCOL=HTTP/1.1  
GATEWAY_INTERFACE=CGI/1.1  

Estas variables permiten que el script entienda la petición.

---

### 2️⃣ Crear comunicación con el script

Debe:

- Crear pipes
- Hacer fork()
- En el hijo: ejecutar el script con execve()
- En el padre:
  - Enviar body por stdin (si es POST)
  - Leer stdout del script

---

### 3️⃣ Leer la salida del script

El script devolverá algo como:

Content-Type: text/html

<html>Hola</html>

El servidor debe:

- Separar headers y body
- Construir respuesta HTTP válida:

HTTP/1.1 200 OK  
Content-Type: text/html  
Content-Length: X  

<html>Hola</html>

---

### 4️⃣ Devolver una Response al sistema principal

El módulo CGI no debe enviar directamente al socket.

Debe devolver un objeto Response que el loop principal enviará al cliente.

---

## ⚠️ Consideración importante: Non-blocking

Nuestro servidor funciona con poll().

CGI no debe bloquear el loop principal.  
Por lo tanto:

- No debemos usar read() bloqueante.
- No debemos usar waitpid() bloqueante.
- Los pipes del CGI deben integrarse en el poll principal.

Primera versión recomendada:
- Implementar versión funcional aunque sea bloqueante.
- Luego adaptarla al modelo non-blocking del servidor.

---

## 📌 Flujo general del CGI

1. El servidor recibe una petición HTTP.
2. Detecta que la ruta corresponde a CGI.
3. Construye variables de entorno.
4. Crea pipes.
5. Hace fork().
6. El hijo ejecuta el script.
7. El padre envía el body (si existe).
8. El padre lee la salida del script.
9. Construye una Response.
10. El loop principal envía la respuesta al cliente.

---

## 🚀 Objetivo mínimo inicial

Primera versión:

- Soportar GET.
- Ejecutar script.
- Leer salida completa.
- Devolver Response correcta.

Después:

- Añadir soporte POST.
- Manejo de errores.
- Integración completa non-blocking con poll().
- Control de timeout.
