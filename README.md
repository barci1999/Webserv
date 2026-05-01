# Webservform

* This project has been created as part of the 42 curriculum by pablalva, ksudyn, rodralva.*

---

## 🧠 Description

Webservform is a custom HTTP/1.1 server written in C++ as part of the 42 Webserv project.

The goal of this project is to understand how a real web server works internally by implementing:
- core features such as request parsing.
- Routing.
- CGI execution.
- Non-blocking.

The server is inspired by NGINX and configuration syntax, allowing flexible server and location blocks.

---

## ⚙️ Features

- HTTP/1.1 request handling
- Methods supported: GET, POST, DELETE
- Static file serving (HTML, CSS, images)
- Multiple servers and ports
- Routing (Location)
- Custom error pages
- File uploads (POST)
- Directory listing (autoindex)
- CGI execution (.py or.php)
- Non-blocking architecture using poll()

## Bonus
- Multiple CGI handler using extension
    - Python (.py)
    - PHP (.php)
---

## 🚀 Instructions

Clone the repository in your local machine

```
git@vogsphere-v2.42madrid.com:vogsphere/intra-uuid-8eb33789-5b7c-40e9-82e0-1fb5c14a6a8c-7215130-pablalva
```

Next you go the folder and run the make command

```
cd Webserver && make
```

Here, you will run the Webserver using a .txt file (it must comply with the same rules as an NGINX .conf file).

Run with your desired .txt file.

```
./webserver conf_curl_test.txt
```

So. a our basic configuration will look like this

```text

    listen 1024;
    root Parseo_solo_toca_Pablo/www;
	index index.html;
	host 127.0.0.1;

    client_max_body_size 10000;

    location / {
		root Parseo_solo_toca_Pablo/www;
		index index.html;
        allowed_methods GET;
    }

    location /upload {
        root Parseo_solo_toca_Pablo/www/upload;
        allowed_methods POST;
        upload_enable on;
		autoindex on;
    }
	location /cgi-bin {
		root Parseo_solo_toca_Pablo/www/test/cgi-bin;
		cgi_extension .py .php;
		cgi_pass  usr/bin/python3 usr/bin/php;
		autoindex on;
	}

    location /delete {
        root Parseo_solo_toca_Pablo/www/upload;
        allowed_methods DELETE;
    }
```

For a server configuration, you will have to specify a server object and list at lest these directives:

- Listen: port where the server listens (e.g. 8080)
- Host: IPv4 address of the server (e.g. 127.0.0.1)
- Server Name: identifier used for virtual hosting
- Root: directory where server files are stored
- Index: default file served when accessing a directory
- Client max body size: maximum allowed size for request body
- Location: defines routing rules for a specific URI and allowed HTTP methods (GET, POST, DELETE)


## Documentation

# CGI 
    https://datatracker.ietf.org/doc/html/rfc3875#section-6.1

```bash
make
