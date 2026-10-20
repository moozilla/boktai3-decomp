#include "global.h"

void CellObject_Init(u32, u8 *);

u32 SceneBody_SetMovementProbe(u8 *p, u32 a, u16 b, u16 c)
{
    *(u32 *)(p + 0x1c) = a;
    CellObject_Init(a, p + 0xc);
    *(u16 *)(p + 0x20) = b;
    *(u16 *)(p + 0x22) = c;
    return 1;
}
