• Definir todos los pares interfaz:puerto en los cuales tu servidor escuchará (definiendo múltiples sitios web servidos por tu programa):

	- hace referencia a la parte de `listen 8080` del bloque de server de nginx.

• Configurar páginas de error por defecto.
	- se le debe inficar una pagina de error por defecto 
	
	- `error_page  404  /404.html;`

• Establecer el tamaño máximo permitido para los cuerpos de las solicitudes de los clientes.
	- se le debe definir el tamamaximo de peticion que puede hacer un cliente ejemplo:

	- `client_max_body_size       10m;`

- indicar ciando se le ponga algo depues de la barra en el url de la pagina como acceder a ese sitiro es decir si quiero acceder a http/werbserver/download definir como,pordonde y que cosas puede hacer en esla agina el cliente
	ejemplo del archivo de configuracion de nginx:

	  `location /download/ {

            valid_referers  none  blocked  server_names  *.example.com;

            if ($invalid_referer) {
                #rewrite   ^/   http://www.example.com/;
                return   403;
            }

            #rewrite_log  on;

            # rewrite /download/*/mp3/*.any_ext to /download/*/mp3/*.mp3
            rewrite ^/(download/.*)/mp3/(.*)\..*$
                    /$1/mp3/$2.mp3                   break;

            root         /spool/www;
            #autoindex    on;
            access_log   /var/log/nginx-download.access_log  download;
        }`

-establecer los metodos http permitidos para esa seccion como en este caso que solo esta permitido GET Y DELETE  pero los demas no estan permitido.
   
    `  location /tienda {
        root /var/test/tienda;
        index index.html;
        autoindex on;
        limit_except GET DELETE {
            deny all;
        }`

- el auto index sirve para el caso en el que se hacee un peticiona una carpteta mostrar los archivos o mandar un error en su defecto

- el caso de que se entre a una carpeta pero no se indique un archivo en especifico de debe mostrar un archivo de forma default tambien se puede poner varios archivos como default y el programa debera buscar en orden ylanzara el primero que exista en relacion con el autoindex cuando alguien pide una carpeta si existe un archivo definido en index se sirve ese archivo ppero si no existe:
	- su autoindex on -> muestra un la lista de arrchivos.
	- si autoindex of -> Error.
 en el caso de que este definicdo un archivo en el index y el outoindex este off da igual se mostrara el archivo igual 
 el auto index solo servira para el caso de que no haya un index definido y se entre en una carpeta 

-en el caso en elque se a un location en el que se le permita usar el metodo POST se le debe indicar el tamaño maximo permitido del archivo 
	ejemplo :

	`location /uploads 
	{  
	   root /var/www;
       client_max_body_size 50M;  
	}`

-el servidor debe poder ejecutar un cgi que significa esto basicamente que cuabdo se haga una peticon de un archivo ejecutable por ejemplo un .php o .py este debe ejecutarlo y mandar la salida a la pagina
    ejemplo:

    `location ~ \.php$ 
    {
        fastcgi_pass 127.0.0.1:9000;
    }`
    la diferencia entre cgi y fast_cgi es que en el primero ser hace un nuevo proceso y se ejecuta el script cada vez que se hace una peticion de  este tipo mientras que con fast_cgi se hace la ejecucion desde un inicio y el proceso de esa ejecucion queda es espera por si hacen las peticiones para devolver la respuesta 