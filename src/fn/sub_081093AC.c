#include "global.h"

extern u32 gUnk_020004B4;

void sub_081093AC(u32 v)
{
    if (gUnk_020004B4 != 0) {
        *(u8 *)(gUnk_020004B4 + 0x23) = v;
    }
}
