#ifndef GUARD_CHALLENGE_RESULT_H
#define GUARD_CHALLENGE_RESULT_H

#include "scene_input.h"

struct ChallengeTimeResult {
    u8 unknown0[0x38];
    struct SceneInputResource digitGraphics;
    u8 unknown58[0x118 - 0x58];
    struct SceneInputWidget digits[6];
    u8 unknown358[0x10];
    s32 elapsedFrames;
    u8 unknown36C[4];
    s32 timer;
};

void ChallengeResult_LoadDigit(struct SceneInputWidget *digit, struct SceneInputResource *graphics,
                               u32 value);
void sub_081C62B4(struct ChallengeTimeResult *result);

#endif
