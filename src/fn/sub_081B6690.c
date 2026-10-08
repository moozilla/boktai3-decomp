#include "global.h"
void sub_081B6604(u8 *);
void sub_081B6668(u8 *);
void sub_081B6690(u8 *p)
{
    u16 *c = (u16 *)(p + 0x98);
    if (*c) {
        (*c)--;
        if (*c == 0) *(u32 *)(p + 0x18) &= ~1;
    } else {
        sub_081B6604(p);
        sub_081B6668(p);
    }
}
