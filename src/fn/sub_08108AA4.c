#include "global.h"

s32 sub_0821ABA8(s32, s32);
u8 *sub_0810853C(void);

void sub_08108AA4(void)
{
    if (sub_0821ABA8(0x6e, 0) != 0) {
        u8 *p = sub_0810853C();
        if (p != 0) {
            *(u16 *)(p + 0x60) = 2;
            p[0x65] = 1;
        }
    }
}
