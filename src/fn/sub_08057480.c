// MATCHING_CONCESSION: register-pinning
#include "level_advance_display.h"
#define LevelAdvanceDisplay_InitFromScript sub_08057480
s32 LevelAdvanceDisplay_InitFromScript(struct LevelAdvanceManager *manager)
{
    s32 level;
    if (Script_SeekToKeyword('e'))
        manager->thresholds = (s32 *)Script_GetValueSafe();
    {
        struct LevelAdvanceProgress *progress = LEVEL_ADVANCE_PROGRESS;
        asm("" : "+r"(progress));
        {
            register s16 *address asm("r1") = &progress->level;
            register s32 sample asm("r0");
            sample = *address;
            if (sample <= 98) {
                s32 argument = sample;
                manager->nextThreshold = sub_08056F20(manager, argument);
            }
        }
    }
    sub_080573F8(manager);
    sub_08057430(manager);
    gUnk_02000120 = manager;
    return 0;
}
