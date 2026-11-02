#include "level_advance_display.h"
s32 LevelAdvanceDisplay_TryAdvanceLevel(struct LevelAdvanceManager *manager)
{
    struct LevelAdvanceProgress *progress = LEVEL_ADVANCE_PROGRESS;
    u16 level = progress->level;
    s16 *levelOut;
    u16 *bonus;
    if (progress->level > 98 || progress->experience < manager->nextThreshold)
        return 0;
    levelOut = &progress->level;
    *levelOut = level + 1;
    bonus = &LEVEL_ADVANCE_PROGRESS->levelBonus;
    *bonus += 2;
    manager->nextThreshold =
        LevelAdvanceDisplay_GetThreshold(manager, LEVEL_ADVANCE_PROGRESS->level);
    return 1;
}
