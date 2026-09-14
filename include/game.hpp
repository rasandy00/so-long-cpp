#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>

class Map;
class Render;

class Game
{
private:
    Map &_map;
    Render &_render;
    SDL_Renderer *_renderer;

    bool _running;
    bool _won;

public:
    Game(Map &map, Render &render, SDL_Renderer *renderer);
    ~Game();

    void run();
    bool hasWon() const;

private:
    void handleEvents();
    void movePlayer(int dx, int dy);
    bool canMove(int x, int y);
    void collectItem(int x, int y);
    bool checkWin(int x, int y);
};

#endif
