#include "global.h"

void Collider_Remove(u8 *);
void sub_08013684(u8 *);
u32 Sprite_Remove(u8 *);

u32 sub_08042B04(u8 *unused, u8 *p)
{
    Collider_Remove(p + 0x80);
    sub_08013684(p + 0x12c);
    return Sprite_Remove(p + 0xe4);
}
