#include "global.h"

void Collider_Remove(u8 *);
u32 Sprite_Remove(u8 *);
void sub_08013B74(u8 *);

void sub_0818F5EC(u8 *p)
{
    Collider_Remove(p + 0x120);
    Sprite_Remove(p + 0x24);
    sub_08013B74(p + 0x60);
}
