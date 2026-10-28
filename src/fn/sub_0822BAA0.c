#include "global.h"
#include "scene_stimulus.h"
void SceneStimulus_ClearExclusionRegions(void)
{
    u8 *p = (u8 *)gSceneStimulusState;
    if (p != 0) {
        s32 lo = (s32)(p + 0x98);
        u32 *q;
        u32 z = 0;
        q = (u32 *)(p + 0xd4);
        do {
            *q = z;
            q--;
        } while ((s32)q >= lo);
    }
}
