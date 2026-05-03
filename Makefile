CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98

NAME := webserver

KONRAD_DIR := Konrad_No_Tocar_Solo_Mirar
PARSEO_DIR := Parseo_solo_toca_Pablo

UPLOAD_DIR := $(PARSEO_DIR)/www/upload

SRC := \
main.cpp \
client.cpp \
listener.cpp \
pollLoop.cpp \
signal.cpp \
utils.cpp \
Logger.cpp\
\
$(KONRAD_DIR)/CGIProcess.cpp \
$(KONRAD_DIR)/Request.cpp \
$(KONRAD_DIR)/RequestParser.cpp \
$(KONRAD_DIR)/Response.cpp \
$(KONRAD_DIR)/ResponseCGI.cpp \
\
$(PARSEO_DIR)/Block.cpp \
$(PARSEO_DIR)/Parser.cpp \
$(PARSEO_DIR)/server.cpp

OBJ := $(SRC:.cpp=.o)

all: create_dirs $(NAME)

create_dirs:
	mkdir -p $(UPLOAD_DIR)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re create_dirs