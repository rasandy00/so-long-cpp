#include "../include/render.hpp"
#include "../include/map.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>

Render::Render(SDL_Renderer *renderer)
    : _renderer(renderer),
      _floor(NULL),
      _wall(NULL),
      _collectible(NULL),
      _exit(NULL),
      _player(NULL),
      _tileSize(32)
{
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

    if (_player != NULL)
        SDL_DestroyTexture(_player);

    IMG_Quit();
}

bool Render::loadTextures()
{
    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        std::cerr << "Erreur IMG_Init : "
                  << IMG_GetError() << std::endl;
        return false;
    }

    _floor = IMG_LoadTexture(_renderer, "image/0.png");
    if (_floor == NULL)
    {
        std::cerr << "Erreur chargement 0.png : "
                  << IMG_GetError() << std::endl;
        return false;
    }

    _wall = IMG_LoadTexture(_renderer, "image/1.png");
    if (_wall == NULL)
    {
        std::cerr << "Erreur chargement 1.png : "
                  << IMG_GetError() << std::endl;
        return false;
    }

    _collectible = IMG_LoadTexture(_renderer, "image/C.png");
    if (_collectible == NULL)
    {
        std::cerr << "Erreur chargement C.png : "
                  << IMG_GetError() << std::endl;
        return false;
    }

    _exit = IMG_LoadTexture(_renderer, "image/E.png");
    if (_exit == NULL)
    {
        std::cerr << "Erreur chargement E.png : "
                  << IMG_GetError() << std::endl;
        return false;
    }

    _player = IMG_LoadTexture(_renderer, "image/P.png");
    if (_player == NULL)
    {
        std::cerr << "Erreur chargement P.png : "
                  << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

void Render::renderMap(const Map &map)
{
    std::string mapData = map.getMap();

    int x = 0;
    int y = 0;

    SDL_Rect destination;

    destination.w = _tileSize;
    destination.h = _tileSize;

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

        /*
         * On dessine toujours le sol en premier,
         * puis l'élément par-dessus si nécessaire.
         */
        SDL_RenderCopy(_renderer, _floor, NULL, &destination);

        if (mapData[i] == '1')
        {
            SDL_RenderCopy(_renderer, _wall, NULL, &destination);
        }
        else if (mapData[i] == 'C')
        {
            SDL_RenderCopy(_renderer, _collectible, NULL, &destination);
        }
        else if (mapData[i] == 'E')
        {
            SDL_RenderCopy(_renderer, _exit, NULL, &destination);
        }
        else if (mapData[i] == 'P')
        {
            SDL_RenderCopy(_renderer, _player, NULL, &destination);
        }

        x++;
    }
}
