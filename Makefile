SRC = 	src/main.cpp \
		src/window.cpp \
		src/game.cpp \
		src/map.cpp \
		src/render.cpp \
		src/trap.cpp \
		src/player.cpp

NAME = game

OBJ = $(SRC:.cpp=.o)

CXX = c++

CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -Iinclude

SDL_FLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_image

all: $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(SDL_FLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re

