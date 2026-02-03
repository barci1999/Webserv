# Archivo de funciones permitidas

 - ## gestion de procesos:
	**-fork:** funcion la cual crea un nuevo proceso duplicando el actual, este nuevo proceso comparte inicialmente codigo y datos por eso es iportante tener en cuenta herencia de memoria dinamica alocada y fds abiertos.
	- prototipo: `pid_t fork(void);`
	- valores de retorno: 
		- ` > 0` proceso padre;
		- ` = 0 ` proceso hijo;
		- ` = -1 ` error no se pudo creear el proceso;

	**-execve:** ejecuta un programa atraves que se encuentra en el pathname,si cuando se ejecuta execve se realiza correctamente cuando termina la ejecucion del programa designado se termina el proceso automanticamente,mientras que si falla la ejecucion se deber terminar el proceso de forma manual
	- prototipo: `int execve(const char *pathname, char *const argv[],char *const envp[]);`
	- parametros:
		- `const char *pathname`: ruta de archivos del programa a ejecutar.
		- ` char **const argv`: matriz de argumentos para el programa a ejecutar.
		- ` char **const envp`: matriz de string que trabajara conmo environment del programa.
	
	**-waitpid:** funcion que se utiliza en el proceso padre para esperar al resultado del proceso hijo ya sea exitosa la ejecucion de lo que haga o no.
	- protptipo: `pid_t waitpid(pid_t pid, int *wstatus, int options);`
	- parametros:
		- `pid_t pid`: id del proceso al que va a esperar:
			- `> 0`: espera al proceso con ese pid en concreto.
			- `= -1`:espera a cualquier proceso hijo.
			- `= 0`: espera a cualquier proceso hijo del mismo grupo.
			- `< -1`:espera a cualquier hijo del grupo `|pid|`.
		- `int *wstatus`:puntero de int donde el kernel guarda como termino el proceso.
			- `WIFEXITED(status)`:el hijo termino normalmente.
			- `WEXITSTATUS(status)`:el hijo termino atraves de un `exit(x)`.
			- `WIFSIGNALED(status)`:el proceso se termino con una señal.
			- `WTERMSIG(status)`: que señar mato al proceso.
		- `int options`:indica el tipo de espera:
			- `0`:espera de forma bloqueante.
			- `WNOHANG`:no bloquea,devuelve:
				- `0`:si el hijo esta vivio.
				- `PID`:si termino l proceso.
	- valores de retorno:
		- `> 0`:el PID del proceso hijo a cambiado por lo cual a terminado ya sea de forma correcta o incorrecta.
		- `= 0`:solo ocurre si se usa `WNOHAN` indica que el hijo sigue ejecutandose.
		- `= -1`:Error se produce cuando no hay hijos,el pid es invalido o cuando se produce un error en el sistema.

	**-kill**: