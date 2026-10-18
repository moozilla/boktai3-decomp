// COMPILER: old_agbcc
#include "global.h"
/* StatusScreen_ClearActive: Clears gUnk_02000044 (the live status screen pointer), the same global StatusScreen_Destroy/Initialize set. */
#define StatusScreen_ClearActive sub_0800E3D4

extern u32 gUnk_02000044;

void sub_0800E3D4(void)
{
    gUnk_02000044 = 0;
}
