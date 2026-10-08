#include "global.h"
extern u32 gUnk_030053F4;
u32 sub_08056D04(void)
{
    if (gUnk_030053F4 & 0x200) return TRUE;
    return FALSE;
}
