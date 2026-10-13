#include "global.h"

void Anim_Step(u8 *, u8 *);

void sub_081E28D4(u8 *p)
{
    Anim_Step(p + 0x98, p + 0x18);
    Anim_Step(p + 0xf8, p + 0x18);
    Anim_Step(p + 0x1b8, p + 0x18);
    (*(u16 *)(p + 0x21e))++;
}
