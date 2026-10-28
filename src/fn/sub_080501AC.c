#include "cell_gate.h"
s32 CellGate_TestPlayerEdge(struct CellGate *gate, s32 limit)
{
    u32 value, mask;
    if (gate->flags & 8)
        value = (u16)gPlayers->position.x;
    else
        value = (u16)gPlayers->position.z;
    mask = 255;
    mask &= value;
    if ((s32)mask <= limit)
        return 1;
    return 0;
}
