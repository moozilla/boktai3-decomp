#include "global.h"
void sub_0821AD08(u32, u32);
void sub_080B309C(u8 *p)
{
    u32 *q = (u32 *)(p + 0x688);
    if (*q != 0) {
        sub_0821AD08(*q, 0);
        *q = 0;
    }
}
