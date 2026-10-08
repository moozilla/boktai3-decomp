#include "global.h"
void sub_08020D68(u8 *, u32);
void sub_081D174C(u8 *a, u8 *p)
{
    if (p[8] != 0) {
        p[8] = 0;
        p[7] = 0;
        p[5] = 0xf;
    }
    if (p[7] != 0) {
        p[5] = 0x12;
        sub_08020D68(p + 0x168, 1);
    }
}
