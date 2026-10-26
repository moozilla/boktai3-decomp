// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* TextWindow_ClearSingleton: Clears the global pointer to the scrolling text window */
#define TextWindow_ClearSingleton sub_08032ED0

void sub_08032ED0(void)
{
    gScrollingDialogue = 0;
}
