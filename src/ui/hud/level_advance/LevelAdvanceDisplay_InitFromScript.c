#include "ui/hud/level_advance_display.h"
s32 LevelAdvanceDisplay_InitFromScript(struct LevelAdvanceManager *manager)
{
    if (Script_SeekToKeyword('e'))
        manager->thresholds = (s32 *)Script_GetValueSafe();
    if (LEVEL_ADVANCE_PROGRESS->level <= 98)
        manager->nextThreshold =
            LevelAdvanceDisplay_GetThreshold(manager, LEVEL_ADVANCE_PROGRESS->level);
    LevelAdvanceDisplay_InitSprite(manager);
    LevelAdvanceDisplay_InitParticles(manager);
    gLevelAdvanceManager = manager;
    return 0;
}
