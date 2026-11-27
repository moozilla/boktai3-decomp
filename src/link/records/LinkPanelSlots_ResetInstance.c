// COMPILER: old_agbcc
#include "global.h"
#include "engine/ram.h"
/* LinkPanelSlots_ResetInstance: Clears the singleton pointer */

void LinkPanelSlots_ResetInstance(void)
{
    gLinkPanel = 0;
}
