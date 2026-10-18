#include "global.h"
void Storage_SetGridFlags(u8 *, u32);
u8 SceneInput_GetGridSelection(u8 *);
void Storage_InitSelectionGrid(u8 *p, u32 a, u32 b, u32 c, u32 d)
{
    u32 z = 0;
    p[0] = a;
    Storage_SetGridFlags(p, d);
    p[1] = b;
    p[2] = c;
    p[3] = SceneInput_GetGridSelection(p);
    p[8] = z;
}
