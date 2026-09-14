#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SDL2/SDL.h>

class Window
{
    private :
        SDL_Window *_window;
        SDL_Renderer *_renderer;

        int _width;
        int _height;
    public:
        Window(int width, int height, const char *title);
        ~Window();

        SDL_Window *getWindow() const;
        SDL_Renderer *getRenderer() const;

        int getWidth() const;
        int getHeight() const;
};


#endif
