#include "global.h"

extern const u16 gUnk_0861410C[];

void sub_08219574(u32 a, u32 b, u32 c)
{
    *(vu16 *)0x04000050 = gUnk_0861410C[a] | 0x40;
    *(vu16 *)0x04000052 = (b << 8) | c;
    *(vu16 *)0x04000054 = 0;
}
