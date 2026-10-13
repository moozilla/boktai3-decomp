#include "global.h"
void Sprite_Remove(u8 *);
void sub_0805AD18(u8 *p)
{
    u8 *q;
    s32 j;
    Sprite_Remove(p);
    q = p + 0x68;
    j = 7;
    do {
        Sprite_Remove(q);
        q += 0x68;
        j--;
    } while (j >= 0);
    q = p + 0x3a8;
    j = 5;
    do {
        Sprite_Remove(q);
        q += 0x68;
        j--;
    } while (j >= 0);
}
