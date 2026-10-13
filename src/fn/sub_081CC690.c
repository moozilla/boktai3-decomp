#include "global.h"
void Trigger_ConsumePending(void *, s32);
void sub_081CC690(void *unused, u8 *p)
{
    u32 one;
    u32 zero;
    if (p[8] != 0) {
        p[8] = 0;
        p[7] = 0;
        p[5] = 14;
    }
    if (p[7] != 0) {
        one = 1;
        zero = 0;
        p[4] = one;
        *(u32 *)(p + 0x14) = zero;
        p[8] = one;
        Trigger_ConsumePending(p + 0x168, 1);
    }
    *(s32 *)(p + 0x14) += 1;
}
