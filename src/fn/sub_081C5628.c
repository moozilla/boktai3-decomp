#include "global.h"

void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_081C55D8(u8 *, void (*)(void));
void sub_081C580C(void);

void sub_081C5628(u8 *p)
{
    sub_082279A8(1, 5, 4, 4, 4, 0xFFFF, 0);
    sub_081C55D8(p, sub_081C580C);
}
