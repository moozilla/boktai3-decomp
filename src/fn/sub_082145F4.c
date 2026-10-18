#include "global.h"
/* Sprite_SetAnimDataIfPresent: Replaces the sprite data pointer at offset 0xC only when the supplied pointer is nonzero. */
#define Sprite_SetAnimDataIfPresent sub_082145F4

void sub_082145F4(u8 *p, u32 v)
{
    if (v)
        *(u32 *)(p + 0xc) = v;
}
