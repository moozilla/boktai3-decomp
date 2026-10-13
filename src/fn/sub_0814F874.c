#include "global.h"

void Trigger_ConsumePending(u8 *, s32);

void sub_0814F874(u8 *p)
{
    u8 *flag = p + 0x59F;

    if (*flag != 0) {
        Trigger_ConsumePending(p + 0x2F8, 1);
        *flag = 0;
    }
}
