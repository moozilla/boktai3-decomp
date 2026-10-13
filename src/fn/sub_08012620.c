#include "global.h"

void Sprite_Remove(u32 p);

void sub_08012620(u32 unused, u8 *p)
{
    Sprite_Remove((u32)(p + 0x64));
}
