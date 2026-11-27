#include "objects/passage_barriers/cell_gate.h"
s32 PassageBarrier_TestPassage(struct CellGate *p)
{
    struct Player *a = gPlayers[0];
    u32 r;
    if (a->tetherMode == 4) {
        u32 t = a->coordinateAxis;
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
            if (gPlayers[0]->actionMode != 4)
                goto no;
        }
        if (PassageBarrier_TestPlayerEdge(p, 0x28) == 0)
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
