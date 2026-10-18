// COMPILER: old_agbcc
#include "global.h"
/* TextWindowManager_ResetInstance: Clears the text window manager singleton pointer */
#define TextWindowManager_ResetInstance sub_080335D4

extern u32 gUnk_020000E4;

void sub_080335D4(void)
{
    gUnk_020000E4 = 0;
}
