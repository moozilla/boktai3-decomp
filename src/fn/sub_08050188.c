#include "cell_gate.h"
s32 CellGate_TestPlayerHeight(struct CellGate *gate)
{
    if (gUnk_02000580->position.y == (s32)(gate->height << 8))
        return 1;
    return 0;
}
