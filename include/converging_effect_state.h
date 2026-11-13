#ifndef GUARD_CONVERGING_EFFECT_STATE_H
#define GUARD_CONVERGING_EFFECT_STATE_H
#include "scene_vec.h"
struct ConvergingParticle {
    u32 flags;
    u8 unknown4[0xb], renderMode, unknown10[8];
    struct SceneVec position;
    u8 unknown20[8];
    u8 active, ticks, speed, kind;
    s16 offsetX;
    u8 unknown2E[2];
    s16 offsetZ;
    u8 unknown32[2];
};
struct ConvergingEffectState {
    void *graphics;
    u8 active, next, ticks, unknown7;
    struct ConvergingParticle particles[4];
};
#endif
