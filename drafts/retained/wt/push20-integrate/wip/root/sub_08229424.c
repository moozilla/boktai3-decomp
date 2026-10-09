#include "global.h"
extern u8 gUnk_03005430[];
u32 sub_08229424(void)
{
    if (gUnk_03005430[4] <= 2) return 1;
    return 0;
}
