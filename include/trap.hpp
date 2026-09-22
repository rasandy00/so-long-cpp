#ifndef TRAP_HPP
#define TRAP_HPP

#include <SDL2/SDL.h>

/*
 * Les valeurs correspondent aux images :
 * TRAP_RETRACTED -> image/2_0.png (sans épine)
 * TRAP_HALF      -> image/2_1.png (semi-épines)
 * TRAP_EXTENDED  -> image/2_2.png (épines sorties)
 *
 * TRAP_STATE_COUNT n'est pas un état : c'est le nombre d'états.
 */
enum TrapState
{
    TRAP_RETRACTED,
    TRAP_HALF,
    TRAP_EXTENDED,
    TRAP_STATE_COUNT
};

/*
 * Renvoie l'état du piège après elapsedMs millisecondes.
 * Cycle de 5 secondes : 2_0 (1s) -> 2_1 (1s) -> 2_2 (2s) -> 2_1 (1s) -> ...
 */
TrapState getTrapState(Uint32 elapsedMs);

bool isTrapDangerous(TrapState state);

#endif
