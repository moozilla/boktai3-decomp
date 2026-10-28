#include "cell_gate.h"
s32 CellGate_TestTraversalMode(struct CellGate *gate)
{
    u32 x;
    u32 w = gPlayerPassageState;
    x = 0;
    if (w != 0) {
        if (gPlayerInteractionMode == 5)
            x = 1;
    }
    if (x == 0) {
        u32 t = gPlayers->actionMode;
        if (t == 3)
            goto yes;
        if (t != 4)
            goto no;
    }
yes:
    return 1;
no:
    return 0;
}
