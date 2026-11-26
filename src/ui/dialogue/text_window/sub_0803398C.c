#include "global.h"
/* TextWindow_SetText: Sets the interpreter text pointer and clears script sequencing and entry state */
#define TextWindow_SetText sub_0803398C

u8 *TextWindow_Find(u32);

s32 sub_0803398C(u32 id, u32 value)
{
    u8 *w = TextWindow_Find(id);
    u8 *q;
    if (w == NULL)
        return -1;
    q = w + 0x60;
    *(u32 *)(w + 0x14) = 0;
    *(u16 *)(w + 0x10) = 0;
    *(u32 *)(q + 0x1c) = value;
    q[8] = 0;
    q[7] = 0;
    return 0;
}
