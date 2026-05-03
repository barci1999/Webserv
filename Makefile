# ============================
# Makefile Webserver 42
# ============================
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98

# -----------------------------
# Carpetas
# -----------------------------
KONRAD_DIR := Konrad_No_Tocar_Solo_Mirar
PARSEO_DIR := Parseo_solo_toca_Pablo

# -----------------------------
# Archivos principales (mains)
# -----------------------------
MAIN_REQUEST := main_parseo_request
MAIN_RESPONSE := main_motaje_response
MAIN_CONF := main_parser_conf
MAIN_CGI := main_cgi_test
MAIN_LOOP := main_loop

# -----------------------------
# Utils (AÑADIDO)
# -----------------------------
UTILS_OBJ := utils.o

# -----------------------------
# Fuentes Konrad
# -----------------------------
KONRAD_SRC := $(wildcard $(KONRAD_DIR)/*.cpp)
KONRAD_OBJ := $(KONRAD_SRC:.cpp=.o)

# -----------------------------
# Fuentes Parseo
# -----------------------------
PARSEO_SRC := $(filter-out $(PARSEO_DIR)/main.cpp, $(wildcard $(PARSEO_DIR)/*.cpp))
PARSEO_OBJ := $(PARSEO_SRC:.cpp=.o)

# -----------------------------
# Fuentes Loop
# -----------------------------
LOOP_SRC := $(filter-out main_%.cpp, $(wildcard *.cpp))
LOOP_OBJ := $(LOOP_SRC:.cpp=.o)

# -----------------------------
# Objetos mains
# -----------------------------
MAIN_REQUEST_OBJ := main_parseo_request.o
MAIN_RESPONSE_OBJ := main_motaje_response.o
MAIN_CONF_OBJ := main_parser_conf.o
MAIN_CGI_OBJ := main_cgi_test.o
MAIN_LOOP_OBJ := main_loop.o

# ============================
# Targets
# ============================
all: request response conf cgi

request: $(MAIN_REQUEST)
response: $(MAIN_RESPONSE)
conf: $(MAIN_CONF)
cgi: $(MAIN_CGI)
loop: $(MAIN_LOOP)

# -----------------------------
# Ejecutables (AQUÍ ESTÁ EL FIX)
# -----------------------------
$(MAIN_REQUEST): $(MAIN_REQUEST_OBJ) $(KONRAD_OBJ) $(PARSEO_OBJ) $(UTILS_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(MAIN_RESPONSE): $(MAIN_RESPONSE_OBJ) $(KONRAD_OBJ) $(PARSEO_OBJ) $(UTILS_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(MAIN_CONF): $(MAIN_CONF_OBJ) $(KONRAD_OBJ) $(PARSEO_OBJ) $(UTILS_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(MAIN_CGI): $(MAIN_CGI_OBJ) $(KONRAD_OBJ) $(PARSEO_OBJ) $(UTILS_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(MAIN_LOOP): $(MAIN_LOOP_OBJ) $(KONRAD_OBJ) $(PARSEO_OBJ) $(LOOP_OBJ) $(UTILS_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# -----------------------------
# Compilación genérica
# -----------------------------
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -----------------------------
# Compilación de Konrad
# -----------------------------
$(KONRAD_DIR)/%.o: $(KONRAD_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -----------------------------
# Compilación de Parseo
# -----------------------------
$(PARSEO_DIR)/%.o: $(PARSEO_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -----------------------------
# Limpiar
# -----------------------------
clean:
	rm -f *.o $(KONRAD_OBJ) $(PARSEO_OBJ) $(UTILS_OBJ) \
	$(MAIN_REQUEST_OBJ) $(MAIN_RESPONSE_OBJ) $(MAIN_CONF_OBJ) \
	$(MAIN_REQUEST) $(MAIN_RESPONSE) $(MAIN_CONF) $(MAIN_CGI) \
	$(MAIN_LOOP) $(MAIN_LOOP_OBJ)

.PHONY: all request response conf cgi clean