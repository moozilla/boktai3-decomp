#include "global.h"

void Trigger_ConsumePending(u8 *, u32);

void sub_08226920(u8 *p)
{
    Trigger_ConsumePending(p + 0x20, 1);
}
