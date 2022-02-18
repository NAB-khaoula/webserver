
NAME = webserv
CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =	main.cpp\
		parsing/Location.cpp\
		parsing/Server.cpp\
		parsing/hundle-cgi.cpp\
		parsing/tools.cpp\
		server/Server.cpp\
		server/Sockets.cpp\
		requestResponse/Request.cpp\
		requestResponse/Response.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME)

%.o: %.cpp
	@$(CC) -c $^ -o $@

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

