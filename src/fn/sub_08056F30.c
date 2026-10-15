// MATCHING_CONCESSION: register-pinning
#include "level_advance_display.h"
s32 LevelAdvanceDisplay_TryAdvanceLevel(struct LevelAdvanceManager *manager)
{
    struct LevelAdvanceProgress *progress = LEVEL_ADVANCE_PROGRESS;
    register u16 level asm("r5") = progress->level;
    if (progress->level > 98 || progress->experience < manager->nextThreshold)
        return 0;
    progress->level = level + 1;
    asm("" : : "r"(level) : "memory");
    LEVEL_ADVANCE_PROGRESS->levelBonus += 2;
    asm("" : : : "memory");
    manager->nextThreshold =
        LevelAdvanceDisplay_GetThreshold(manager, LEVEL_ADVANCE_PROGRESS->level);
    return 1;
}
