NAME = webserv
CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC =	main.cpp\
		parsing/Location.cpp\
		parsing/Server.cpp\
		parsing/hundle-cgi.cpp\
		parsing/tools.cpp\
		server/Server.cpp\
		requestResponse/Request.cpp\
		requestResponse/Response.cpp

INC = 	WebServ.hpp\
		parsing/Location.hpp\
		parsing/Server.hpp\
		requestResponse/Request.hpp\
		requestResponse/Response.hpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME)

%.o: %.cpp $(INC)
	@$(CC) -c $< -o $@

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all
