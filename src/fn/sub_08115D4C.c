// COMPILER: old_agbcc
#include "global.h"
/* Collectible_ClearManager: Clears the singleton collectible manager pointer */
#define Collectible_ClearManager sub_08115D4C

extern u32 gUnk_020001E4;

void sub_08115D4C(void)
{
    gUnk_020001E4 = 0;
}
