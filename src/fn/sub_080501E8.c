#include "cell_gate.h"
s32 CellGate_TestPassage(struct CellGate *p)
{
    struct CellGatePlayer *a = gPlayers;
    u32 r;
    if (a->transitionMode == 4) {
        u32 t = a->direction;
        if (t == 1) {
            if (p->flags & 0x10)
                goto yes;
        }
        if (t == 7) {
            if (p->flags & 8)
                goto yes;
        }
        goto no;
    } else {
        u32 f, v;
        if (a->actionMode == 3)
            goto yes;
        v = gPlayerPassageState;
        f = 0;
        if (v != 0) {
            if (gPlayerInteractionMode == 5)
                f = 1;
        }
        if (f == 0) {
            if (gPlayers->actionMode != 4)
                goto no;
        }
        if (CellGate_TestPlayerEdge(p, 0x28) == 0)
            goto no;
    }
yes:
    r = 1;
    goto end;
no:
    r = 0;
end:
    return r;
}
