// COMPILER: old_agbcc
#include "global.h"
#include "ram.h"
/* ControlPanel_ClearActivePanel: Clears the global panel handle used to start an interaction */
#define ControlPanel_ClearActivePanel sub_081C17C8

void sub_081C17C8(void)
{
    gSegmentDisplay = 0;
}
