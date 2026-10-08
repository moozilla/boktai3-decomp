#include "global.h"

void sub_08217074(u32 a, u32 b)
{
    *(vu16 *)0x04000052 = (b << 8) | a;
}
