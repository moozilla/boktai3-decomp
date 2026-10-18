// COMPILER: old_agbcc
#include "global.h"
/* TextWindow_ClearSingleton: Clears the global pointer to the scrolling text window */
#define TextWindow_ClearSingleton sub_08032ED0

extern u32 gUnk_020000E0;

void sub_08032ED0(void)
{
    gUnk_020000E0 = 0;
}
