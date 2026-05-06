#include "Response.hpp" // Asegúrate de que el nombre coincida con tu header
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

/* ========================================================================== */
/*                      CONSTRUCTORES Y DESTRUCTOR                            */
/* ========================================================================== */

Response::Response() : _version("HTTP/1.1"), _statusCode(200), _reasonPhrase("OK"), _body("") {}

Response::Response(std::string v, unsigned int s, std::string r, std::map<std::string, std::string> h, std::string b)
    : _version(v), _statusCode(s), _reasonPhrase(r), _headers(h), _body(b) {}

Response::Response(const Response& other) { *this = other; }

Response::Response(const Request to_check, const server server_config) {
    this->_version = "HTTP/1.1";
    this->_statusCode = to_check.get_status_code();
    this->_reasonPhrase = to_check.get_final_status();

    this->_headers.clear();
    this->_body.clear();

    if (this->_statusCode == 200 || this->_statusCode == 201 || this->_statusCode == 204) {
        std::string method = to_check.get_method();
        if (method == "POST")            make_Post(to_check, server_config);
        else if (method == "GET")        make_Get(to_check, server_config);
        else if (method == "DELETE")     make_Delete(to_check, server_config);
        else                             set_error(*this, 405, server_config);
    } else {
        set_error(*this, this->_statusCode, server_config);
    }
}

Response::~Response() {}

Response& Response::operator=(const Response& other) {
    if (this != &other) {
        this->_version = other.get_version();
        this->_statusCode = other.get_statusCode();
        this->_reasonPhrase = other.get_reasonPhrase();
        this->_headers = other.get_headers();
        this->_body = other.get_body();
    }
    return *this;
}

/* ========================================================================== */
/*                      MÉTODOS HTTP PRINCIPALES                              */
/* ========================================================================== */

void Response::make_Get(const Request to_check, const server server_config) {
    std::string path = to_check.get_path();
    if (path.empty()) return set_error(*this, 404, server_config);

    Block best_location = find_best_location(path, server_config);
    std::string full_path = build_full_path(path, best_location, server_config);

    if (!file_exist(full_path)) return set_error(*this, 404, server_config);
    if (!can_read(full_path))   return set_error(*this, 403, server_config);

    if (is_directory(full_path)) {
        handle_directory_get(full_path, path, best_location, server_config);
    } else {
        handle_file_get(full_path, server_config);
    }
}

void Response::make_Delete(const Request to_check, const server server_config) {
    std::string path = to_check.get_path();
    if (path.empty()) return set_error(*this, 404, server_config);
    
    Block best_loc = find_best_location(path, server_config);
    if (best_loc.getName().empty() || !is_method_allowed("DELETE", best_loc)) {
        return set_error(*this, 405, server_config);
    }
    
    std::string full_path = build_full_path(path, best_loc, server_config);
    
    if (!file_exist(full_path))  return set_error(*this, 404, server_config);
    if (is_directory(full_path)) return set_error(*this, 403, server_config);
    
    std::string parent_path = take_parent_path(full_path);
    if (!can_write(parent_path)) return set_error(*this, 403, server_config);
    
    if (std::remove(full_path.c_str()) != 0) {
        return set_error(*this, 500, server_config);
    }
    
    finalize_response(204, "text/plain", "");
}

void Response::make_Post(const Request to_check, const server server_config) {
    std::string path = to_check.get_path();
    if (path.empty()) return set_error(*this, 404, server_config);

    Block loc = find_best_location(path, server_config);
    if (loc.getName().empty() || !is_method_allowed("POST", loc)) {
        return set_error(*this, 405, server_config);
    }

    Directive upload = search_directive("upload_enable", loc);
    if (upload.name.empty() || std::find(upload.args.begin(), upload.args.end(), "on") == upload.args.end()) {
        return set_error(*this, 403, server_config);
    }

    if (to_check.get_body().size() > static_cast<size_t>(server_config.get_srvClientMaxBody())) {
        return set_error(*this, 413, server_config);
    }

    handle_upload_post(to_check, loc, path, server_config);
}

/* ========================================================================== */
/*                      MANEJO DE ERRORES Y RESPUESTAS                        */
/* ========================================================================== */

void Response::set_error(Response& modifi, unsigned int error, const server& server_config) {
    modifi._statusCode = (select_valuePhrase(error) == "Not Implemented") ? 501 : error;
    modifi._reasonPhrase = select_valuePhrase(modifi._statusCode);

    std::string body;
    std::string error_path = get_error_page_path(modifi._statusCode, server_config);

    if (!error_path.empty() && file_exist(error_path) && can_read(error_path)) {
        read_file(error_path, body);
    } else {
        body = gen_error_body(modifi._statusCode);
    }

    modifi.finalize_response(modifi._statusCode, "text/html", body);
}

void Response::finalize_response(int status, const std::string& contentType, const std::string& body) {
    this->_version = "HTTP/1.1";
    this->_statusCode = status;
    this->_reasonPhrase = select_valuePhrase(status);
    this->_body = body;
    
    this->_headers.clear();
    if (!contentType.empty()) {
        addback_headers("Content-Type", contentType);
    }
    addback_headers("Content-Length", toString(body.size()));
    addback_headers("Connection", "close");
}

/* ========================================================================== */
/*                      HELPERS PRIVADOS DE LÓGICA                            */
/* ========================================================================== */

std::string Response::build_full_path(const std::string& path, Block& loc, const server& config) {
    Directive root = search_directive("root", loc);
    if (root.name.empty() || root.args.empty()) {
        root = config.get_srvRoot();
    }
    if (root.args.empty()) return "";

    std::string root_path = root.args[0];
    size_t max_len = loc.getName().length();
    std::string relative = path.substr(max_len);
    return root_path + relative;
}

bool Response::is_method_allowed(const std::string& method, Block& loc) {
    Directive methods = search_directive("allowed_methods", loc);
    if (methods.name.empty() || methods.args.empty()) return false;
    return std::find(methods.args.begin(), methods.args.end(), method) != methods.args.end();
}

void Response::handle_file_get(const std::string& full_path, const server& config) {
    std::string body;
    if (!is_file(full_path)) return set_error(*this, 403, config);
    if (!read_file(full_path, body))
	{ 
		std::cout<<"GGGGGGGGGGGGG"<<std::endl;
		return set_error(*this, 500, config);
	}

    
    finalize_response(200, getContentType(full_path), body);
}

void Response::handle_directory_get(std::string full_path, const std::string& req_path, Block& loc, const server& config) {
    if(full_path[full_path.size() - 1] != '/') full_path += '/';
    
    Directive index = search_directive("index", loc);
    std::string body;

    // 1. Intentar servir el archivo index
    if (!index.name.empty()) {
        for (size_t i = 0; i < index.args.size(); ++i) {
            std::string temp = full_path + index.args[i];
            if (file_exist(temp) && is_file(temp) && can_read(temp)) {
                if (!read_file(temp, body)){ std::cout<<"LLLLLLLLLLL"<<std::endl; return set_error(*this, 500, config);}
                return finalize_response(200, getContentType(temp), body);
            }
        }
    }

    // 2. Si no hay index, intentar autoindex
    Directive autoindex = search_directive("autoindex", loc);
    if (autoindex.name.empty()) autoindex = config.get_srvAutoindex();

    if (!autoindex.name.empty() && !autoindex.args.empty() && autoindex.args[0] == "on") {
        body = generate_autoindex(full_path, req_path);
        if (body.empty()) {std::cout<<"ZZZZZZZZZZZZZZZZZ"<<std::endl; return set_error(*this, 500, config);}
        return finalize_response(200, "text/html", body);
    }

    // 3. Si falla todo, Forbidden
    set_error(*this, 403, config);
}

void Response::handle_upload_post(const Request to_check, Block& loc, const std::string& path, const server& config) {
    std::string content_type = to_check.get_a_header("content-type");
    if (content_type.empty()) return set_error(*this, 415, config);
    
    std::string boundry = extract_boundry(content_type);
    if (boundry.empty()) return set_error(*this, 415, config);
    
    std::string file_name = extract_filename(to_check.get_body());
    if (file_name.empty()) return set_error(*this, 400, config);
    
    std::string clean_body = extract_body(to_check.get_body(), boundry);

    std::string full_path = build_full_path(path, loc, config);
    if (full_path[full_path.size() - 1] != '/') full_path += '/';
    full_path += file_name;
	std::cout<<full_path<<std::endl;

    int fd_file = 0;
    int status_code = 201; // Created

    if (file_exist(full_path)) {
        if (is_directory(full_path)) return set_error(*this, 403, config);
        if (is_file(full_path) && can_write(take_parent_path(full_path))) {
            fd_file = open(full_path.c_str(), O_WRONLY | O_TRUNC);
            status_code = 200; // OK (Overwrite)
        } else {
            return set_error(*this, 403, config);
        }
    } else {
        fd_file = open(full_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }

    if (fd_file == -1) {std::cout << "FFFFFFFFFFFFFF"<<std::endl; return set_error(*this, 500, config);}

    size_t total_bits = 0;
    while (total_bits < clean_body.size()) {
        ssize_t w = write(fd_file, clean_body.c_str() + total_bits, clean_body.size() - total_bits);
        if (w <= 0) {
            close(fd_file);
			std::cout<<"GGGGGGGGGGGGGGGG"<<std::endl;
            return set_error(*this, 500, config);
        }
        total_bits += w;
    }
    close(fd_file);

    finalize_response(status_code, "", "");
}

std::string Response::get_error_page_path(unsigned int code, const server& config) {
    std::vector<Directive> errors = config.get_srvErrorPage();
    for (size_t i = 0; i < errors.size(); ++i) {
        if (errors[i].args.size() >= 2 && errors[i].args[0] == toString(code)) {
            std::string path = errors[i].args[1];
            if (!path.empty() && path[0] == '/') path.erase(0, 1);
            return path;
        }
    }
    return "";
}

/* ========================================================================== */
/*                      PARSEO Y UTILIDADES (MIME, AUTOINDEX)                 */
/* ========================================================================== */

std::string Response::select_valuePhrase(unsigned int status) {
    switch (status) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 415: return "Unsupported Media Type";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 505: return "HTTP Version Not Supported";
        default:  return "Not Implemented";
    }
}

std::string gen_error_body(unsigned int error) {
    std::ostringstream oss;
    oss << "<h1>" << error << " - Error</h1>";
    switch (error) {
        case 400: return "<h1>400 - Bad Request</h1>";
        case 403: return "<h1>403 - Forbidden</h1>";
        case 404: return "<h1>404 - Not Found</h1>";
        case 405: return "<h1>405 - Method Not Allowed</h1>";
        case 413: return "<h1>413 - Payload Too Large</h1>";
        case 415: return "<h1>415 - Unsupported Media Type</h1>";
        case 500: return "<h1>500 - Internal Server Error</h1>";
        case 501: return "<h1>501 - Not Implemented</h1>";
        default:  return oss.str();
    }
}

std::string Response::generate_autoindex(const std::string& full_path, const std::string& request_path) {
    DIR* dir = opendir(full_path.c_str());
    if (!dir) return "";
    
    std::string body = "<html><head><title>Index of " + request_path + "</title></head><body>";
    body += "<h1>Index of " + request_path + "</h1><ul>";
    
    dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        body += "<li><a href=\"" + name + "\">" + name + "</a></li>";
    }
    body += "</ul></body></html>";
    closedir(dir);
    return body;
}

std::string Response::getContentType(const std::string& path) {
    size_t pos = path.rfind('.');
    if (pos == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(pos + 1);
    if (ext == "html" || ext == "htm") return "text/html";
    if (ext == "css") return "text/css";
    if (ext == "js")  return "application/javascript";
    if (ext == "json") return "application/json";
    if (ext == "txt") return "text/plain";
    if (ext == "png") return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "gif") return "image/gif";
    if (ext == "svg") return "image/svg+xml";
    if (ext == "ico") return "image/x-icon";
    if (ext == "pdf") return "application/pdf";

    return "application/octet-stream";
}

std::string Response::extract_boundry(const std::string& raw_boundry) {
    size_t pos = raw_boundry.find("boundary=");
    if (pos == std::string::npos) return "";
    return raw_boundry.substr(pos + 9);
}

std::string Response::extract_filename(const std::string& raw_body) {
    if (raw_body.empty()) return "";
    size_t pos = raw_body.find("filename=\"");
    if (pos == std::string::npos) return "";
    pos += 10;
    size_t end = raw_body.find("\"", pos);
    if (end == std::string::npos) return "";
    return raw_body.substr(pos, end - pos);
}

std::string Response::extract_body(const std::string& body, const std::string& boundary) {
    size_t start = body.find("\r\n\r\n");
    if (start == std::string::npos) return "";
    start += 4;
    
    size_t end = body.find("\r\n--" + boundary, start);
    if (end == std::string::npos) return "";
    
    std::string content = body.substr(start, end - start);
    if (content.size() >= 2 && content.substr(content.size() - 2) == "\r\n")
        content = content.substr(0, content.size() - 2);
    return content;
}

std::string res_to_str(const Response& to_change) {
    std::ostringstream res;
    res << to_change.get_version() << " " << to_change.get_statusCode() << " " << to_change.get_reasonPhrase() << "\r\n";
    
    std::map<std::string, std::string> headers = to_change.get_headers();
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
        res << it->first << ": " << it->second << "\r\n";
    }
    res << "\r\n" << to_change.get_body();
    return res.str();
}