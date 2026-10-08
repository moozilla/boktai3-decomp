#include "global.h"

extern u32 gUnk_03005278;

u32 sub_08219C00(void)
{
    gUnk_03005278 = gUnk_03005278 * 0x5D588B65 + 1;
    return gUnk_03005278 & 0x7FFF;
}
