#include "global.h"
s16 *TetherActor_GetPosition(u8 *);
void TetherActor_RequestKindNineAction(u16, u8);

bool32 sub_081A398C(u8 *p)
{
    s16 *pos = TetherActor_GetPosition(p);
    s32 dx = pos[0] - *(s16 *)(p + 0x1c);
    s32 dz = pos[2] - *(s16 *)(p + 0x20);
    bool32 r;
    if (dx * dx + dz * dz > 0x3fff) {
        r = FALSE;
    } else {
        TetherActor_RequestKindNineAction(*(u16 *)(p + 0x7c), p[0x163]);
        r = TRUE;
    }
    return r;
}
