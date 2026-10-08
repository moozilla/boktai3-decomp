#include "global.h"

void sub_081C2838(u8 *);

void sub_081C2BE8(u8 *p)
{
    sub_081C2838(p);
    *(u32 *)(p + 0x770) += 1;
}
