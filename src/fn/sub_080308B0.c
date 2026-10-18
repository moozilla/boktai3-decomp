// COMPILER: old_agbcc
#include "global.h"
/* MessageWindow_ResetInstance: Clears the message window singleton pointer used by MessageWindow_OpenFromScript */
#define MessageWindow_ResetInstance sub_080308B0

extern u32 gUnk_020000DC;

void sub_080308B0(void)
{
    gUnk_020000DC = 0;
}
