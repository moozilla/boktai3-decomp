// COMPILER: old_agbcc
#include "global.h"
/* SaveMenu_ClearSingleton: Clears the global menu task pointer */
#define SaveMenu_ClearSingleton sub_08110A8C

extern u32 gUnk_020001D4;

void sub_08110A8C(void)
{
    gUnk_020001D4 = 0;
}
