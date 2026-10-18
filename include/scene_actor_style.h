#ifndef GUARD_SCENE_ACTOR_STYLE_H
#define GUARD_SCENE_ACTOR_STYLE_H
#include "global.h"
/* Tentative style descriptor, also initialized by 0811D428. */
struct SceneActorStyle {
    u8 adjustedA, adjustedB, adjustedC, parameterD, parameterE, parameterA;
    u8 unknown6[2], entries[4], count, parameterB, parameterF, parameterC;
    s16 initialHalfword, currentHalfword;
    u32 initialWord, currentWord;
};
#endif
