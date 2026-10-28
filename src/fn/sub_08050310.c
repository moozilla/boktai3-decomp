#include "cell_gate.h"
s32 CellGate_TestForcedPassage(struct CellGate *p)
{
    struct CellGatePlayer *g = gUnk_02000580;
    if (g->transitionMode == 4) {
        u32 v = g->direction;
        if (v == 1) {
            if (p->flags & 0x10)
                goto yes;
        }
        if (v == 7) {
            if (p->flags & 8)
                goto yes;
        }
        goto no;
    } else {
        u32 x;
        u32 w = gUnk_02000534;
        x = 0;
        if (w != 0) {
            if (gPlayerInteractionMode == 5)
                x = 1;
        }
        if (x == 0) {
            u32 t = gUnk_02000580->actionMode;
            if (t == 3)
                goto call;
            if (t != 4)
                goto no;
        }
    call:
        if (CellGate_TestPlayerEdge(p, 0x28) != 0)
            goto yes;
        goto no;
    }
yes:
    return 1;
no:
    return 0;
}
