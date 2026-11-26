// COMPILER: old_agbcc
#include "global.h"
#include "objects/items/item_container.h"
/* TriggerManager_Reset: nulls the global TriggerManager pointer gTriggerManager without tearing down any trigger list, placed as a segment-boundary reset alongside choice menu' */

void TriggerManager_Reset(void)
{
    gTriggerManager = 0;
}
