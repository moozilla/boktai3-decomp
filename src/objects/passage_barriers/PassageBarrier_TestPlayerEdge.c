#include "objects/passage_barriers/cell_gate.h"
s32 PassageBarrier_TestPlayerEdge(struct CellGate *gate, s32 limit)
{
    u32 value, mask;
    if (gate->flags & 8)
        value = (u16)gPlayers[0]->position.x;
    else
        value = (u16)gPlayers[0]->position.z;
    mask = 255;
    mask &= value;
    if ((s32)mask <= limit)
        return 1;
    return 0;
}
