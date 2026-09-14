#include "../include/window.hpp"
#include <iostream>

Window::Window(int width, int height, const char *title)
    : _window(NULL),
      _renderer(NULL),
      _width(width),
      _height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Erreur SDL_Init : "
                  << SDL_GetError() << std::endl;
        return;
    }

    _window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        _width,
        _height,
        SDL_WINDOW_SHOWN
    );

    if (_window == NULL)
    {
        std::cerr << "Erreur SDL_CreateWindow : "
                  << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    _renderer = SDL_CreateRenderer(
        _window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (_renderer == NULL)
    {
        std::cerr << "Erreur SDL_CreateRenderer : "
                  << SDL_GetError() << std::endl;

        SDL_DestroyWindow(_window);
        _window = NULL;

        SDL_Quit();
        return;
    }
}

Window::~Window()
{
    if (_renderer != NULL)
        SDL_DestroyRenderer(_renderer);

    if (_window != NULL)
        SDL_DestroyWindow(_window);

    SDL_Quit();
}

SDL_Window *Window::getWindow() const
{
    return _window;
}

SDL_Renderer *Window::getRenderer() const
{
    return _renderer;
}

int Window::getWidth() const
{
    return _width;
}

int Window::getHeight() const
{
    return _height;
}


/*
SDL_Window	*creatWindow(const char *title, int width, int heith)
{
	SDL_Window	*window;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr <<"Erreur SDL_Init : "<< SDL_GetError() << std::endl;
		return (NULL);
	}
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, heith, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		std::cerr <<"Erreur SDL_CreatWindow :"<< SDL_GetError() << std::endl;
		SDL_Quit();
		return (NULL); 
	}
	return (window); 
}*/
