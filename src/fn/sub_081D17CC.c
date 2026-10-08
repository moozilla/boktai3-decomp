#include "global.h"
void sub_08020D68(u8 *, u32);
void sub_081D17CC(u8 *a, u8 *p)
{
    u8 *q = p + 0x20;
    if (p[8] != 0) {
        p[8] = 0;
        p[7] = 0;
        p[5] = 0x12;
        sub_08020D68(p + 0x168, 1);
        *(u16 *)(q + 0x16) = 1;
    }
}
