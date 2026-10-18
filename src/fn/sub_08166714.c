#include "global.h"
struct P {
    u8 f0[0x4D9C];
    s16 a;
    s16 b;
};
void PauseMenu_SmoothPoint(u8 *, u8 *, s32);
void PauseMenu_InterpolateActivePairPositions(struct P *p, s32 n)
{
    if (p->a >= 0)
        PauseMenu_SmoothPoint((u8 *)p + 0x4200, (u8 *)p + 0x442C, n);
    if (p->b >= 0)
        PauseMenu_SmoothPoint((u8 *)p + 0x4260, (u8 *)p + 0x4434, n);
}
