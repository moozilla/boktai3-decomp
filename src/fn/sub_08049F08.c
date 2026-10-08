#include "global.h"
extern u8 *gUnk_02000488;
u32 sub_08049F08(void)
{
    u8 *p = gUnk_02000488;
    u32 r;
    if (p) r = p[0x3E4];
    else r = 0;
    return r;
}
