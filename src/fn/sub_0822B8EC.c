#include "global.h"
#include "scene_stimulus.h"
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
u32 Script_GetPc(void);
void sub_0822B640(void);
u32 SceneStimulus_InitFromScript(u8 *p)
{
    if (Script_SeekToKeyword(0x61)) {
        u32 *q = (u32 *)(p + 0x98);
        while (Script_GetPc())
            *q++ = Script_GetValue();
    }
    sub_0822B640();
    gSceneStimulusState = (struct SceneStimulusState *)p;
    return 1;
}
