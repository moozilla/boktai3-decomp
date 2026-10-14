#include "global.h"

extern u8 *gUnk_02000710;

void sub_08159228(u8 *p)
{
    *(u32 *)(gUnk_02000710 + p[0x472] * 4 + 0x628) = 0;
}
