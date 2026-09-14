#ifndef MAP_HPP
#define MAP_HPP

#include <string>

struct Position
{
    int x;
    int y;
};

class Map
{
private:
    std::string _map;
    int _length;
    int _width;
    Position _player;
    Position _exit;
    int _collectibles;

public:
    Map();
    ~Map();

    std::string getMap() const;
    int getLength() const;
    int getWidth() const;
    Position getPlayerPosition() const;
    Position getExitPosition() const;
    int getCollectibles() const;

    void setMap(const std::string &map);
    void setLength(int length);
    void setWidth(int width);
    void setPlayerPosition(int x, int y);
    void setExitPosition(int x, int y);
    void setCollectibles(int number);

    bool loadMap(const std::string &filename);
    bool isValid() const;
};

#endif
