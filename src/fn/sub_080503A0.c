#include "cell_gate.h"
void CellGate_ClampPlayerEdge(struct CellGate *p, s32 b)
{
    if (p->flags & 8) {
        u32 v = (u16)gPlayers[0]->position.x;
        if ((s32)(v & 0xff) >= b) {
            gPlayers[0]->position.x = ((s16)v >> 8 << 8) + b;
            {
                u16 t = gPlayers[0]->position.x;
                gPlayers[0]->colliderPosition.x = t;
            }
        }
    } else {
        u32 v = (u16)gPlayers[0]->position.z;
        if ((s32)(v & 0xff) >= b) {
            gPlayers[0]->position.z = ((s16)v >> 8 << 8) + b;
            {
                u16 t = gPlayers[0]->position.z;
                gPlayers[0]->colliderPosition.z = t;
            }
        }
    }
}
