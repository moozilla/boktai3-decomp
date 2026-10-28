#include "global.h"
#include "scene_stimulus.h"
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
s32 SceneStimulus_FindExclusionRegionSlot(u8 *, s32);
/* Keyword 0x61: add each listed id that is not in the table yet (a negative
 * result is the free slot, as -1 - index). */
void SceneStimulus_AddExclusionRegionsFromScript(void)
{
    u8 *s = (u8 *)gSceneStimulusState;
    if (s && Script_SeekToKeyword(0x61)) {
        while (Script_GetPc()) {
            s32 id = Script_GetValue();
            s32 slot = SceneStimulus_FindExclusionRegionSlot(s, id);
            if (slot < 0)
                ((struct SceneStimulusState *)s)->ids[-slot] = id;
        }
    }
}
