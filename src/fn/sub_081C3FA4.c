#include "global.h"

extern u32 gUnk_02000268;
void Sprite_Remove(u8 *);

void sub_081C3FA4(u8 *p)
{
    Sprite_Remove(p + 0x34);
    gUnk_02000268 = 0;
}
