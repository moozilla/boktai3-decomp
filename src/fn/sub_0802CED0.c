#include "global.h"
void DisplayAnimation_InitializeSequence(u8 *, s32, s32, s32, u8 *);
u32 DisplayAnimation_GetTileIndex(u8 *);
void sub_0802CED0(u8 *p, s32 a, s32 b, s32 c)
{
    u8 *q = p + 0x184;
    DisplayAnimation_InitializeSequence(q, a, b, c, p + 0xC);
    *(u32 *)(p + 0x68) = DisplayAnimation_GetTileIndex(q);
}
