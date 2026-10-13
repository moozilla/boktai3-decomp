#include "global.h"
void Trigger_ConsumePending(u8 *, u32);
void sub_081D1778(u8 *a, u8 *p)
{
    if (p[8] != 0) {
        p[8] = 0;
        p[7] = 0;
        p[5] = 0x10;
    }
    if (p[7] != 0) {
        p[5] = 0x12;
        Trigger_ConsumePending(p + 0x168, 1);
    }
}
