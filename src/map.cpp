#include "../include/map.hpp"
#include <fstream>

Map::Map()
    : _map(""),
      _length(0),
      _width(0),
      _collectibles(0)
{
    /*
     * En C++98 on ne peut pas écrire _player {0, 0} dans la
     * liste d'initialisation (syntaxe C++11). On initialise
     * donc les positions dans le corps du constructeur.
     */
    _player.x = 0;
    _player.y = 0;
    _exit.x = 0;
    _exit.y = 0;
}

Map::~Map()
{}

std::string Map::getMap() const
{
    return (_map);
}

int Map::getLength() const
{
    return _length;
}

int Map::getWidth() const
{
    return _width;
}

Position Map::getPlayerPosition() const
{
    return _player;
}

Position Map::getExitPosition() const
{
    return _exit;
}

int Map::getCollectibles() const
{
    return _collectibles;
}

void Map::setMap(const std::string &map)
{
    _map = map;
}

void Map::setLength(int len)
{
    _length = len;
}

void Map::setWidth(int width)
{
    _width = width;
}

void Map::setPlayerPosition(int x, int y)
{
    _player.x = x;
    _player.y = y;
}

void Map::setExitPosition(int x, int y)
{
    _exit.x = x;
    _exit.y = y;
}

void Map::setCollectibles(int nbr)
{
    _collectibles = nbr;
}

bool Map::loadMap(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        return false;

    std::string line;
    int y = 0;
    int collectibles = 0;

    _map.clear();
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        _map += line;
        _map += '\n';

        for (int x = 0; x < (int)line.length(); x++)
        {
            if (line[x] == 'P')
            {
                _player.x = x;
                _player.y = y;
            }
            else if (line[x] == 'E')
            {
                _exit.x = x;
                _exit.y = y;
            }
            else if (line[x] == 'C')
                collectibles++;
        }
        _width = line.length();
        y++;
    }
    _length = y;
    _collectibles = collectibles;
    file.close();

    /*
     * On vérifie que la map respecte les règles du jeu :
     * bordure complète, un seul joueur, une seule sortie,
     * au moins un collectible, aucun caractère invalide.
     */
    if (!isValid())
        return false;

    return true;
}

bool Map::isValid() const
{
    int width = _width;
    int length = _length;

    int playerCount = 0;
    int exitCount = 0;
    int collectibleCount = 0;

    int x = 0;
    int y = 0;

    for (int i = 0; i < (int)_map.length(); i++)
    {
        char c = _map[i];

        if (c == '\n')
        {
            /* Chaque ligne doit avoir exactement la même largeur. */
            if (x != width)
                return false;

            x = 0;
            y++;
            continue;
        }

        /* Seuls ces caractères sont autorisés dans une map. */
        if (c != '0' && c != '1' && c != 'P' && c != 'E' && c != 'C')
            return false;

        /* La map doit être entourée d'une bordure complète de murs. */
        if (x == 0 || x == width - 1 || y == 0 || y == length - 1)
        {
            if (c != '1')
                return false;
        }

        if (c == 'P')
            playerCount++;
        else if (c == 'E')
            exitCount++;
        else if (c == 'C')
            collectibleCount++;

        x++;
    }

    if (playerCount != 1)
        return false;

    if (exitCount != 1)
        return false;

    if (collectibleCount < 1)
        return false;

    return true;
}
