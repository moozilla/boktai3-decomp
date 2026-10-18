#include "global.h"
u32 Save_AlignSizeToEightBytes(u32);
u32 sub_0822F0A4(u32, u32, u32);
extern u32 gUnk_030053F8;
u32 Save_WriteHeader(void)
{
    u32 n = Save_AlignSizeToEightBytes(0x28);
    if (sub_0822F0A4(0, gUnk_030053F8, n) != n + 8)
        return 0;
    return 1;
}
