#include "global.h"
extern u8 *gUnk_020004A4;
void sub_0805C470(void)
{
    if (gUnk_020004A4 != 0) {
        u8 *p;
        u8 t;
        gUnk_020004A4[0xc0f] = 1;
        p = gUnk_020004A4;
        t = p[0xc10];
        p[0xc11] = t;
    }
}
