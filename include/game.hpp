#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include "trap.hpp"
#include "player.hpp"
#include "map.hpp"

class Render;

class Game
{
private:
    Map &_map;
    Render &_render;
    SDL_Renderer *_renderer;

    bool _running;
    bool _won;
    bool _quit;

    int _lives;

    Uint32 _trapStartTime;
    TrapState _trapState;

    Uint32 _hitTime;
    Uint32 _blinkUntil;

    /*
     * Animation de marche : le joueur glisse visuellement de
     * _moveFrom vers _moveTo pendant que la logique (position dans
     * Map, collisions) est déjà à jour depuis le début du mouvement.
     */
    bool _isMoving;
    Uint32 _moveStartTime;
    Position _moveFrom;
    Position _moveTo;
    PlayerDirection _facing;

    /*
     * _idleFrame vaut 0 ou 3 (les deux pieds au sol) et alterne à
     * chaque déplacement. _moveStartFrame/_moveSwingFrame décrivent
     * le pas en cours : pieds joints -> pied qui avance -> pieds
     * joints (dans l'autre posture).
     */
    int _idleFrame;
    int _moveStartFrame;
    int _moveSwingFrame;

public:
    Game(Map &map, Render &render, SDL_Renderer *renderer, int lives);
    ~Game();

    void run();
    bool hasWon() const;
    bool hasQuit() const;
    int getLives() const;

private:
    void handleEvents();
    void movePlayer(int dx, int dy);
    bool canMove(int x, int y);
    void collectItem(int x, int y);
    bool checkWin(int x, int y);
    void updateTraps();
    void checkTrapCollision();
    void hurtPlayer();
    void startMoveAnimation(Position from, int toX, int toY, int dx, int dy);
    void getRenderState(PlayerDirection &dir, int &frame,
                         int &pixelX, int &pixelY);
};

#endif
