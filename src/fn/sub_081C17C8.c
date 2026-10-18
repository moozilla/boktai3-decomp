// COMPILER: old_agbcc
#include "global.h"
/* ControlPanel_ClearActivePanel: Clears the global panel handle used to start an interaction */
#define ControlPanel_ClearActivePanel sub_081C17C8

extern u32 gUnk_02000260;

void sub_081C17C8(void)
{
    gUnk_02000260 = 0;
}
