// COMPILER: old_agbcc
#include "global.h"
/* TriggerManager_Reset: nulls the global TriggerManager pointer gUnk_02000030 without tearing down any trigger list, placed as a segment-boundary reset alongside choice menu' */
#define TriggerManager_Reset sub_08006EF4

extern u32 gUnk_02000030;

void sub_08006EF4(void)
{
    gUnk_02000030 = 0;
}
