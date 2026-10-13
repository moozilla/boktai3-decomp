#include "global.h"
void Sprite_Remove(u8 *);
void sub_0812632C(u8 *p)
{
    Sprite_Remove(p);
    p[0x6c] = 0xff;
    p[0x58] = 0;
}
