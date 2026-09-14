#ifndef RENDER_HPP
#define RENDER_HPP

#include <SDL2/SDL.h>
#include <string>

class Map;

class Render
{
private:
    SDL_Renderer *_renderer;

    SDL_Texture *_floor;
    SDL_Texture *_wall;
    SDL_Texture *_collectible;
    SDL_Texture *_exit;
    SDL_Texture *_player;

    int _tileSize;

public:
    Render(SDL_Renderer *renderer);
    ~Render();

    bool loadTextures();
    void renderMap(const Map &map);
};

#endif
