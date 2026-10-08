#include "global.h"

extern u8 *gUnk_020004B4;

void sub_08109374(void)
{
    if (gUnk_020004B4) {
        gUnk_020004B4[0x2A] = 1;
    }
}
