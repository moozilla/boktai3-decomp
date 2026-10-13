#include "global.h"
void Sprite_Remove(u8 *);
void Collider_Remove(u8 *);
void sub_08013684(u8 *);
void sub_081B854C(u8 *p)
{
    Sprite_Remove(p + 0x18);
    Collider_Remove(p + 0x94);
    if (*(s16 *)(p + 0xec) == 0xc)
        sub_08013684(p + 0xfc);
}
