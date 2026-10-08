#include "global.h"
extern u8 *gUnk_02000488;
u32 sub_08049DFC(void)
{
    u8 *p = gUnk_02000488;
    if (!p) return 0;
    if (p[0x255] == 1) return 1;
    return 0;
}
