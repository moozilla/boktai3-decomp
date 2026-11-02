#include "cell_gate.h"
void CellGate_ClampPlayerEdge(struct CellGate *p, s32 b)
{
    if (p->flags & 8) {
        u32 v = *(u16 *)&gPlayers->position.x;
        if ((s32)(v & 0xff) >= b) {
            *(u16 *)&gPlayers->position.x = ((s16)v >> 8 << 8) + b;
            {
                u16 t = *(u16 *)&gPlayers->position.x;
                gPlayers->colliderPosition.x = t;
            }
        }
    } else {
        u32 v = *(u16 *)&gPlayers->position.z;
        if ((s32)(v & 0xff) >= b) {
            *(u16 *)&gPlayers->position.z = ((s16)v >> 8 << 8) + b;
            {
                u16 t = *(u16 *)&gPlayers->position.z;
                gPlayers->colliderPosition.z = t;
            }
        }
    }
}
