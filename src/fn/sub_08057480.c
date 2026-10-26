#include "level_advance_display.h"
s32 LevelAdvanceDisplay_InitFromScript(struct LevelAdvanceManager *manager)
{
    s32 level;
    if (Script_SeekToKeyword('e')) {
        manager->thresholds = (s32 *)Script_GetValueSafe();
    }
    {
        struct LevelAdvanceProgress *progress = LEVEL_ADVANCE_PROGRESS;
        {
            s16 *address = &progress->level;
            s32 sample;
            sample = *address;
            if ((*address) <= 98) {
                s32 argument = *address;
                manager->nextThreshold = LevelAdvanceDisplay_GetThreshold(manager, argument);
            }
        }
    }
    LevelAdvanceDisplay_InitSprite(manager);
    LevelAdvanceDisplay_InitParticles(manager);
    gLevelAdvanceManager = manager;
    return 0;
}
