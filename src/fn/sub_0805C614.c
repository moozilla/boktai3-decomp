#include "global.h"
extern u8 *gUnk_020004A4;
void sub_0805C614(void)
{
    u8 *g = gUnk_020004A4;
    if (g != 0) {
        u8 *a = g + 0x1337;
        u32 z = 0;
        *a = 1;
        *(u16 *)(gUnk_020004A4 + 0x1338) = z;
    }
}
