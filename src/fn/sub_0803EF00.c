#include "global.h"

void Sprite_Remove(u8 *);
void sub_0803F7D8(u8 *);
void sub_0811B394(u8 *);
void sub_0803EF00(u32 a, u8 *p)
{
    Sprite_Remove(p + 0xb4);
    sub_0803F7D8(p + 0xf0);
    sub_0811B394(p + 0x58);
}
