SRC = 	src/main.cpp \
		src/window.cpp \
		src/game.cpp \
		src/map.cpp \
		src/render.cpp 

NAME = game

OBJ = $(SRC:.cpp=.o)

CXX = c++

SDL_CFLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude $(SDL_CFLAGS)

all: $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(SDL_LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re
