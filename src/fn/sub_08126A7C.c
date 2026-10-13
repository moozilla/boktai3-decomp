#include "global.h"
void Sprite_Remove(u8 *);
void Memory_Clear(u8 *, u32);
void sub_08126A7C(u32 a, u8 *p)
{
    if (p[4] != 0)
        Sprite_Remove(p);
    p[0x3c] = 0;
    Memory_Clear(p, 0x40);
}
