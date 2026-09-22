#include "../include/map.hpp"
#include <fstream>

Map::Map() :
    _map (""),
    _length (0),
    _width (0),
    _collectibles (0)
{
    _player.x = 0;
    _player.y = 0;
    _exit.x = 0;
    _exit.y = 0;
    _start.x = 0;
    _start.y = 0;
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

Position Map::getStartPosition() const
{
    return _start;
}

int Map::getCollectibles() const
{
    return _collectibles;
}

bool Map::isTrap(int x, int y) const
{
    for (std::size_t i = 0; i < _traps.size(); i++)
    {
        if (_traps[i].x == x && _traps[i].y == y)
            return true;
    }
    return false;
}


void Map::setMap(const std::string map)
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

bool Map::loadMap(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        return false;

    std::string line;
    int y = 0;
    int collectibles = 0;

    _map.clear();
    _traps.clear();
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        _map += line;
        _map += '\n';

        for(int x = 0 ; x < (int)line.length(); x++)
        {
            if (line[x] == 'P')
            {
                _player.x = x;
                _player.y = y;
                _start.x = x;
                _start.y = y;
            }
            else if (line[x] == 'E')
            {
                _exit.x = x;
                _exit.y = y;
            }
            else if (line[x] == 'C')
                collectibles++;
            else if (line[x] == 'T')
            {
                Position trap;
                trap.x = x;
                trap.y = y;
                _traps.push_back(trap);
            }
        }
        _width = line.length();
        y++;
    }
    _length = y;
    _collectibles = collectibles;
    file.close();
    return (true);
}
