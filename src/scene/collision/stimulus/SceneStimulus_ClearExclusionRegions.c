#include "scene/collision/scene_stimulus.h"

void SceneStimulus_ClearExclusionRegions(void)
{
    struct SceneStimulusState *state = gSceneStimulusState;
    if (state) {
        s32 first = (s32)&state->ids[0];
        s32 cursor;
        s32 zero = 0;
        cursor = (s32)&state->ids[15];
        do {
            *(s32 *)cursor = zero;
            cursor -= sizeof(state->ids[0]);
        } while (cursor >= first);
    }
}
