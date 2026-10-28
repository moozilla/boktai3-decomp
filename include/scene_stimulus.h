#ifndef GUARD_SCENE_STIMULUS_H
#define GUARD_SCENE_STIMULUS_H

#include "global.h"

struct SceneStimulus {
    s16 x, y, z;
    u8 unk06[2];
    u16 id; /* 0xFFFF sentinel; signed interpretation in posting/overlap tests */
    u16 range, flags, tag;
};

struct SceneStimulusState {
    u8 unk00[0x18];
    struct SceneStimulus stimuli[8];
    s32 ids[16]; /* exclusion-region ids */
};

extern struct SceneStimulusState *gSceneStimulusState;

#endif
