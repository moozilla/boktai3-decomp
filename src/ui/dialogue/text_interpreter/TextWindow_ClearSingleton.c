// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* TextWindow_ClearSingleton: Clears the global pointer to the scrolling text window */

void TextWindow_ClearSingleton(void)
{
    gScrollingDialogue = 0;
}
