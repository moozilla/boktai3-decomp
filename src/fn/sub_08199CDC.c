#include "global.h"

u32 Sprite_Remove(u8 *);

void sub_08199CDC(u8 *p)
{
    if (*(p + 0x28)) {
        Sprite_Remove(p + 0x24);
    }
}
