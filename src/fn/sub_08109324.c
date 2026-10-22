#include "global.h"
extern u8 *gUnk_020004B4;

void sub_08109324(void)
{
    if (gUnk_020004B4 != 0)
        gUnk_020004B4[0x25] = 1;
}
