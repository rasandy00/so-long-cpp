#include "../include/game.hpp"
#include "../include/map.hpp"
#include "../include/render.hpp"

#include <iostream>
#include <cstdlib>

Game::Game(Map &map, Render &render, SDL_Renderer *renderer)
    : _map(map),
      _render(render),
      _renderer(renderer),
      _running(true),
      _won(false)
{
}

Game::~Game()
{
}

void Game::run()
{
    while (_running)
    {
        handleEvents();

        SDL_SetRenderDrawColor(
            _renderer,
            0,
            0,
            0,
            255
        );

        SDL_RenderClear(_renderer);

        _render.renderMap(_map);

        SDL_RenderPresent(_renderer);

        SDL_Delay(16);
    }
}

void Game::handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            _running = false;
        }

        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                _running = false;
            }
            else if (event.key.keysym.sym == SDLK_UP ||
                     event.key.keysym.sym == SDLK_z)
            {
                movePlayer(0, -1);
            }
            else if (event.key.keysym.sym == SDLK_DOWN ||
                     event.key.keysym.sym == SDLK_s)
            {
                movePlayer(0, 1);
            }
            else if (event.key.keysym.sym == SDLK_LEFT ||
                     event.key.keysym.sym == SDLK_q)
            {
                movePlayer(-1, 0);
            }
            else if (event.key.keysym.sym == SDLK_RIGHT ||
                     event.key.keysym.sym == SDLK_d)
            {
                movePlayer(1, 0);
            }
        }
    }
}

void Game::movePlayer(int dx, int dy)
{
    Position current = _map.getPlayerPosition();

    int newX = current.x + dx;
    int newY = current.y + dy;

    if (!canMove(newX, newY))
        return;

    std::string mapData = _map.getMap();

    int width = _map.getWidth();

    /*
     * Chaque ligne de la map contient :
     *
     * largeur caractères + '\n'
     *
     * Pour trouver une position :
     *
     * index = y * (width + 1) + x
     */

    int oldIndex = current.y * (width + 1) + current.x;
    int newIndex = newY * (width + 1) + newX;

    char destination = mapData[newIndex];

    /*
     * Le joueur quitte son ancienne case.
     */
    mapData[oldIndex] = '0';

    /*
     * Le joueur arrive sur la nouvelle case.
     */
    if (destination == 'C')
    {
        collectItem(newX, newY);
        mapData[newIndex] = 'P';
    }
    else if (destination == 'E')
    {
        /*
         * canMove() n'autorise déjà l'entrée sur la sortie que
         * lorsque tous les collectibles ont été récupérés, donc
         * checkWin() est vrai ici.
         */
        if (checkWin(newX, newY))
        {
            mapData[newIndex] = 'P';
            _map.setMap(mapData);
            _map.setPlayerPosition(newX, newY);

            std::cout << "========================" << std::endl;
            std::cout << "       VICTOIRE !       " << std::endl;
            std::cout << "========================" << std::endl;

            _won = true;
            _running = false;
            return;
        }

        return;
    }
    else
    {
        mapData[newIndex] = 'P';
    }

    _map.setMap(mapData);
    _map.setPlayerPosition(newX, newY);
}

bool Game::canMove(int x, int y)
{
    /*
     * Vérification des limites de la map.
     */
    if (x < 0 || y < 0)
        return false;

    if (x >= _map.getWidth())
        return false;

    if (y >= _map.getLength())
        return false;

    std::string mapData = _map.getMap();

    int width = _map.getWidth();

    /*
     * +1 car chaque ligne possède '\n'.
     */
    int index = y * (width + 1) + x;

    /*
     * Vérification de sécurité.
     */
    if (index < 0 ||
        index >= static_cast<int>(mapData.length()))
    {
        return false;
    }

    /*
     * Le joueur ne peut pas traverser un mur.
     */
    if (mapData[index] == '1')
        return false;

    /*
     * Le joueur ne peut pas entrer dans la sortie
     * tant que tous les collectibles ne sont pas récupérés.
     */
    if (mapData[index] == 'E' &&
        _map.getCollectibles() > 0)
    {
        std::cout << "Il reste "
                  << _map.getCollectibles()
                  << " collectible(s)."
                  << std::endl;

        return false;
    }

    return true;
}

void Game::collectItem(int x, int y)
{
    std::string mapData = _map.getMap();

    int width = _map.getWidth();

    int index = y * (width + 1) + x;

    if (mapData[index] == 'C')
    {
        int remaining = _map.getCollectibles();

        if (remaining > 0)
            remaining--;

        _map.setCollectibles(remaining);

        std::cout << "Collectible récupéré ! "
                  << "Il reste : "
                  << remaining
                  << std::endl;
    }
}

bool Game::checkWin(int x, int y)
{
    Position exitPosition = _map.getExitPosition();

    /*
     * Vérifie que le joueur est bien
     * sur la position de la sortie.
     */
    if (x != exitPosition.x ||
        y != exitPosition.y)
    {
        return false;
    }

    /*
     * Tous les collectibles doivent avoir
     * été récupérés.
     */
    if (_map.getCollectibles() != 0)
        return false;

    return true;
}

bool Game::hasWon() const
{
    return _won;
}
