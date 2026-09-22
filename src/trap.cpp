#include "../include/trap.hpp"

namespace
{
    const Uint32 RETRACTED_MS = 1000;
    const Uint32 HALF_OUT_MS = 1000;
    const Uint32 EXTENDED_MS = 2000;
    const Uint32 HALF_IN_MS = 1000;

    const Uint32 CYCLE_MS = RETRACTED_MS + HALF_OUT_MS
                          + EXTENDED_MS + HALF_IN_MS;
}

TrapState getTrapState(Uint32 elapsedMs)
{
    Uint32 timeInCycle = elapsedMs % CYCLE_MS;

    if (timeInCycle < RETRACTED_MS)
        return TRAP_RETRACTED;
    timeInCycle -= RETRACTED_MS;

    if (timeInCycle < HALF_OUT_MS)
        return TRAP_HALF;
    timeInCycle -= HALF_OUT_MS;

    if (timeInCycle < EXTENDED_MS)
        return TRAP_EXTENDED;

    return TRAP_HALF;
}

bool isTrapDangerous(TrapState state)
{
    return state != TRAP_RETRACTED;
}
