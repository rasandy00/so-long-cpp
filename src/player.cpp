#include "../include/player.hpp"

PlayerDirection directionFromDelta(int dx, int dy)
{
    if (dy < 0)
        return DIR_UP;
    if (dy > 0)
        return DIR_DOWN;
    if (dx < 0)
        return DIR_LEFT;
    return DIR_RIGHT;
}
