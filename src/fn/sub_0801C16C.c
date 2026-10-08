#include "global.h"
struct O { u8 f[8]; u32 fl; };
void sub_0801C16C(u8 *p)
{
    ((struct O *)(p + 0x1a4))->fl |= 1;
}
