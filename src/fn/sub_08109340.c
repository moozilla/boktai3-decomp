#include "global.h"

extern u32 gUnk_020004B4;

void sub_08109340(u32 v)
{
    if (gUnk_020004B4 != 0) {
        *(u8 *)(gUnk_020004B4 + 0x21) = v;
    }
}
