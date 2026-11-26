#include "global.h"
s8 Hud_GetGridSelection(u8 *);
void Hud_SetGridPosition(u8 *p, u8 a, u8 b)
{
    u32 z = 0;
    p[1] = a;
    p[2] = b;
    p[3] = Hud_GetGridSelection(p);
    p[0xb] = z;
}
