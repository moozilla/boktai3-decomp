#include "global.h"
void sub_08217EEC(u8 *, u32, u32);
void sub_08217ECC(u8 *, u32);
void sub_081D7908(u8 *p, u32 b)
{
    u8 *q = p + 0x48;
    s32 i = 3;
    do {
        sub_08217EEC(q, *(u32 *)(p + 0x44), b);
        sub_08217ECC(q, 1);
        q += 0x30;
        i--;
    } while (i >= 0);
}
