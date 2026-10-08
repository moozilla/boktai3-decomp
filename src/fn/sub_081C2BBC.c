#include "global.h"

void sub_081C29A4(u8 *);
void sub_081C2838(u8 *);

void sub_081C2BBC(u8 *p)
{
    u32 *c = (u32 *)(p + 0x770);
    if ((*c & 1) == 0) {
        sub_081C29A4(p);
    }
    sub_081C2838(p);
    *c = *c + 1;
}
