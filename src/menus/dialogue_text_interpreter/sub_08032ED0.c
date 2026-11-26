// COMPILER: old_agbcc
#include "engine/shared/global.h"
#include "engine/shared/ram.h"
/* TextWindow_ClearSingleton: Clears the global pointer to the scrolling text window */
#define TextWindow_ClearSingleton sub_08032ED0

void sub_08032ED0(void)
{
    gScrollingDialogue = 0;
}
