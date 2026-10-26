// COMPILER: old_agbcc
#include "global.h"
#include "item_container.h"
/* TriggerManager_Reset: nulls the global TriggerManager pointer gTriggerManager without tearing down any trigger list, placed as a segment-boundary reset alongside choice menu' */
#define TriggerManager_Reset sub_08006EF4

void sub_08006EF4(void)
{
    gTriggerManager = 0;
}
