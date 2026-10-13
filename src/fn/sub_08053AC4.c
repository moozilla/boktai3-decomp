#include "global.h"
void Sprite_Remove(u8 *);
void sub_08225938(u8 *);
u32 sub_08053AC4(u8 *p)
{
    Sprite_Remove(p + 0x68);
    Sprite_Remove(p + 0xB8);
    if (p[0x1B] != 0)
        sub_08225938(p + 0x20);
    return 0;
}
