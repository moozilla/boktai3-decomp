#include "cell_gate.h"
void CellGate_ClampPlayerEdge(struct CellGate *p, s32 b)
{
    if (p->flags & 8) {
        u32 v = *(u16 *)&gUnk_02000580->position.x;
        if ((s32)(v & 0xff) >= b) {
            *(u16 *)&gUnk_02000580->position.x = ((s16)v >> 8 << 8) + b;
            {
                u16 t = *(u16 *)&gUnk_02000580->position.x;
                *(u16 *)((u8 *)gUnk_02000580 + 0x168) = t;
            }
        }
    } else {
        u32 v = *(u16 *)&gUnk_02000580->position.z;
        if ((s32)(v & 0xff) >= b) {
            *(u16 *)&gUnk_02000580->position.z = ((s16)v >> 8 << 8) + b;
            {
                u16 t = *(u16 *)&gUnk_02000580->position.z;
                *(u16 *)((u8 *)gUnk_02000580 + 0x16c) = t;
            }
        }
    }
}
