#include "objects/cell_gate/cell_gate.h"
s32 CellGate_TestPlayerHeight(struct CellGate *gate)
{
    if (gPlayers[0]->position.y == (s32)(gate->height << 8))
        return 1;
    return 0;
}
