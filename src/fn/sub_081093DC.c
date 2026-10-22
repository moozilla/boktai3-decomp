#include "global.h"
extern u8 *gUnk_020004B4;

bool32 sub_081093DC(void)
{
    if (gUnk_020004B4 == 0 || gUnk_020004B4[0x1e] == 0)
        return FALSE;
    return TRUE;
}
