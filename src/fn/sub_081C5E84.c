// COMPILER: old_agbcc
#include "global.h"

extern u8 *gUnk_02000710;

void sub_081C5E84(void)
{
    *(s32 *)(gUnk_02000710 + 0x658) = -1;
    *(s32 *)(gUnk_02000710 + 0x65c) = -1;
}
