#include "global.h"
extern u16 gUnk_03005320;

bool32 sub_08222C24(void)
{
    if (gUnk_03005320 != 0xffff)
        return TRUE;
    return FALSE;
}
