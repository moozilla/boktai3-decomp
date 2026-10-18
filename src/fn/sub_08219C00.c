#include "global.h"

extern u32 gUnk_03005278;

u32 Random_Next15(void)
{
    gUnk_03005278 = gUnk_03005278 * 0x5D588B65 + 1;
    return gUnk_03005278 & 0x7FFF;
}
