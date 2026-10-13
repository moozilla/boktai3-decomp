#include "global.h"
void Sprite_Remove(u8 *);
void sub_08045550(u32 *a, u32 *b, u32 n)
{
    b[0x78 / 4] |= 1;
    Sprite_Remove((u8 *)b + 0x78);
    a[6] &= ~(1 << n);
}
