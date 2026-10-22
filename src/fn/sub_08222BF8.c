#include "global.h"
extern u16 gUnk_03005320;
extern u16 gUnk_0300530C;

bool32 sub_08222BF8(void)
{
    if ((gUnk_03005320 | gUnk_0300530C) != 0xffff)
        return TRUE;
    return FALSE;
}
