#include "global.h"
void Trigger_ConsumePending(u8 *, u32);
void sub_081CEBB8(u8 *a, u8 *b)
{
    if (b[8]) {
        b[8] = 0;
        b[7] = 0;
        b[5] = 0x17;
    }
    if (b[7]) {
        b[5] = 1;
        Trigger_ConsumePending(b + 0x168, 1);
    }
    *(u32 *)(b + 0x14) += 1;
}
