#include "global.h"
void TextSequence_SetState(u8 *p, u8 v)
{
    u8 *q = p + 0x324;
    u32 z = 0;
    *q = v;
    *(u16 *)(p + 0x322) = z;
}
