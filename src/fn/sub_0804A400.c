// COMPILER: old_agbcc
#include "global.h"

extern u8 *gUnk_02000710;

s16 sub_0804A400(void)
{
    return *(s16 *)(gUnk_02000710 + 0x5aa);
}
