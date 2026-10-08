#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049CB8(u32 a)
{
    u8 *p = gUnk_02000488;
    if (p) p[0x472] = a;
}
