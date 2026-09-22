#ifndef RENDER_HPP
#define RENDER_HPP

#include <SDL2/SDL.h>
#include <string>
#include "trap.hpp"
#include "player.hpp"

class Map;

class Render
{
private:
    SDL_Renderer *_renderer;

    SDL_Texture *_floor;
    SDL_Texture *_wall;
    SDL_Texture *_collectible;
    SDL_Texture *_exit;
    SDL_Texture *_trap[TRAP_STATE_COUNT];
    SDL_Texture *_playerSheet[DIR_COUNT][4];

    int _tileSize;

public:
    Render(SDL_Renderer *renderer);
    ~Render();

    bool loadTextures();
    int getTileSize() const;
    void renderMap(const Map &map, TrapState trapState,
                   bool showFlash, bool playerVisible,
                   PlayerDirection playerDir, int playerFrame,
                   int playerPixelX, int playerPixelY);
};

#endif

