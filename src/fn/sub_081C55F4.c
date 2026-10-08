#include "global.h"

void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_081C55D8(u8 *, void (*)(void));
void sub_081C57E8(void);

void sub_081C55F4(u8 *p)
{
    sub_082279A8(0, 5, 4, 4, 4, 0xFFFF, 0);
    sub_081C55D8(p, sub_081C57E8);
}
