// COMPILER: old_agbcc
#include "global.h"
/* SpriteDrop_ClearManager: Clears the singleton sprite drop manager pointer */
#define SpriteDrop_ClearManager sub_08116280

extern u32 gUnk_020001E8;

void sub_08116280(void)
{
    gUnk_020001E8 = 0;
}
