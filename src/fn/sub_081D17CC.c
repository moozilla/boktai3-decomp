#include "global.h"
void Trigger_ConsumePending(u8 *, u32);
void sub_081D17CC(u8 *a, u8 *p)
{
    u8 *q = p + 0x20;
    if (p[8] != 0) {
        p[8] = 0;
        p[7] = 0;
        p[5] = 0x12;
        Trigger_ConsumePending(p + 0x168, 1);
        *(u16 *)(q + 0x16) = 1;
    }
}
