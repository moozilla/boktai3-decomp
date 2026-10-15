#include "level_advance_display.h"
s32 LevelAdvanceDisplay_GetThreshold(struct LevelAdvanceManager *manager, u32 level)
{
    s32 *thresholds = manager->thresholds;
    return *(s32 *)((u8 *)thresholds + (level << 2)) * 10;
}
