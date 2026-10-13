#include "global.h"

void Collider_Remove(u8 *);
void sub_08013684(u8 *);
void Sprite_Remove(u8 *);
void sub_08044118(u32 a, u8 *p)
{
    Collider_Remove(p + 0x68);
    sub_08013684(p + 0x114);
    Sprite_Remove(p + 0xcc);
}
