#include "../include/render.hpp"
#include "../include/map.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <sstream>

Render::Render(SDL_Renderer *renderer)
    : _renderer(renderer),
      _floor(NULL),
      _wall(NULL),
      _collectible(NULL),
      _exit(NULL),
      _tileSize(32)
{
    for (int i = 0; i < TRAP_STATE_COUNT; i++)
        _trap[i] = NULL;

    for (int d = 0; d < DIR_COUNT; d++)
    {
        for (int f = 0; f < 4; f++)
            _playerSheet[d][f] = NULL;
    }
}

Render::~Render()
{
    if (_floor != NULL)
        SDL_DestroyTexture(_floor);

    if (_wall != NULL)
        SDL_DestroyTexture(_wall);

    if (_collectible != NULL)
        SDL_DestroyTexture(_collectible);

    if (_exit != NULL)
        SDL_DestroyTexture(_exit);

    for (int i = 0; i < TRAP_STATE_COUNT; i++)
    {
        if (_trap[i] != NULL)
            SDL_DestroyTexture(_trap[i]);
    }

    for (int d = 0; d < DIR_COUNT; d++)
    {
        for (int f = 0; f < 4; f++)
        {
            if (_playerSheet[d][f] != NULL)
                SDL_DestroyTexture(_playerSheet[d][f]);
        }
    }
}

bool Render::loadTextures()
{
    _floor = IMG_LoadTexture(_renderer, "image/0.png");
    if (_floor == NULL)
        return false;

    _wall = IMG_LoadTexture(_renderer, "image/1.png");
    if (_wall == NULL)
        return false;

    _collectible = IMG_LoadTexture(_renderer, "image/C.png");
    if (_collectible == NULL)
        return false;

    _exit = IMG_LoadTexture(_renderer, "image/E.png");
    if (_exit == NULL)
        return false;

    /*
     * L'ordre suit l'enum TrapState :
     * TRAP_RETRACTED, TRAP_HALF, TRAP_EXTENDED.
     */
    const char *trapPaths[TRAP_STATE_COUNT] =
    {
        "image/2_0.png",
        "image/2_1.png",
        "image/2_2.png"
    };

    for (int i = 0; i < TRAP_STATE_COUNT; i++)
    {
        _trap[i] = IMG_LoadTexture(_renderer, trapPaths[i]);
        if (_trap[i] == NULL)
            return false;
    }

    /*
     * L'ordre suit l'enum PlayerDirection :
     * DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT.
     * 4 images par direction (0 à 3), comme fournies
     * dans image/Player/.
     */
    const char *dirLetters[DIR_COUNT] = { "F", "B", "L", "R" };

    for (int d = 0; d < DIR_COUNT; d++)
    {
        for (int f = 0; f < 4; f++)
        {
            std::ostringstream path;
            path << "image/Player/P_" << dirLetters[d] << "_" << f << ".png";

            _playerSheet[d][f] = IMG_LoadTexture(_renderer, path.str().c_str());
            if (_playerSheet[d][f] == NULL)
                return false;
        }
    }

    return true;
}

int Render::getTileSize() const
{
    return _tileSize;
}

void Render::renderMap(const Map &map, TrapState trapState,
                        bool showFlash, bool playerVisible,
                        PlayerDirection playerDir, int playerFrame,
                        int playerPixelX, int playerPixelY)
{
    std::string mapData = map.getMap();

    int x = 0;
    int y = 0;

    SDL_Rect destination;

    destination.w = _tileSize;
    destination.h = _tileSize;

    for(std::size_t i = 0; i < mapData.length(); i++)
    {
        if (mapData[i] == '\n')
        {
            x = 0;
            y++;
            continue;
        }
        destination.x = x * _tileSize;
        destination.y = y * _tileSize;
        SDL_RenderCopy(_renderer, _floor, NULL, &destination);
        x++;
    }
        x = 0;
    y = 0;
    for (std::size_t i = 0; i < mapData.length(); i++)
    {
        if (mapData[i] == '\n')
        {
            x = 0;
            y++;
            continue;
        }

        destination.x = x * _tileSize;
        destination.y = y * _tileSize;

        if(mapData[i] == '1')
        {
            SDL_RenderCopy(_renderer, _wall, NULL, &destination);
        }
        else if (mapData[i] == 'C')
        {
            SDL_RenderCopy(_renderer, _floor, NULL, &destination);
            SDL_RenderCopy(_renderer, _collectible, NULL, &destination);
        }
        else if (mapData[i] == 'E')
        {
            SDL_RenderCopy(_renderer, _floor, NULL, &destination);
            SDL_RenderCopy(_renderer, _exit, NULL, &destination);
        }
        else if (mapData[i] == 'T')
        {
            SDL_RenderCopy(_renderer, _trap[trapState], NULL, &destination);
        }
        else if (mapData[i] == 'P')
        {
            /*
             * Le joueur n'est plus dessiné ici : il est dessiné une
             * seule fois plus bas, à sa position en pixels, pour
             * pouvoir glisser librement entre deux cases. On affiche
             * seulement le sol (ou le piège) sous ses pieds.
             */
            if (map.isTrap(x, y))
                SDL_RenderCopy(_renderer, _trap[trapState], NULL, &destination);
            else
                SDL_RenderCopy(_renderer, _floor, NULL, &destination);
        }

        x++;
    }

    /*
     * Le joueur est dessiné à sa position en pixels (et non à une
     * case de la grille), ce qui permet l'animation de marche entre
     * deux cases. Pendant le clignotement qui suit une réapparition,
     * il n'est dessiné qu'une frame sur deux.
     */
    if (playerVisible)
    {
        SDL_Rect playerDestination;

        playerDestination.x = playerPixelX;
        playerDestination.y = playerPixelY;
        playerDestination.w = _tileSize;
        playerDestination.h = _tileSize;

        SDL_RenderCopy(_renderer, _playerSheet[playerDir][playerFrame],
                        NULL, &playerDestination);
    }

    /*
     * Flash noir affiché brièvement au moment de l'impact
     * avec un piège dangereux.
     */
    if (showFlash)
    {
        SDL_Rect overlay;

        overlay.x = 0;
        overlay.y = 0;
        overlay.w = map.getWidth() * _tileSize;
        overlay.h = map.getLength() * _tileSize;

        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 200);
        SDL_RenderFillRect(_renderer, &overlay);
        SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);
    }
}

