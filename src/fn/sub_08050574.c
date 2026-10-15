#include "cell_gate.h"
#define CellGate_TestTraversalMode sub_08050574
s32 sub_08050574(struct CellGate *gate)
{
    u32 x;
    u32 w = gUnk_02000534;
    x = 0;
    if (w != 0) {
        if (gUnk_02000500 == 5)
            x = 1;
    }
    if (x == 0) {
        u32 t = gUnk_02000580->actionMode;
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
