#ifndef GUARD_SPIRAL_BURST_H
#define GUARD_SPIRAL_BURST_H
#include "level_advance_display.h"

/* Each burst owns eight 0x30-byte particles. Their frame and motion data begin
 * at offsets 0x10 and 0x18 within the particle. */
struct SpiralParticleMotion {
    union LevelAdvancePosition position;
    struct SpiralParticle *previous, *next;
    u8 angle, riseRate, riseDelay, animationFrame, angularSpeed, lifetime;
    u16 radius;
};

struct SpiralParticleFrame {
    u8 status, listIndex, unknown12[2];
    u32 frameState;
    struct SpiralParticleMotion motion;
};

struct SpiralParticle {
    u32 flags;
    u8 unknown4[11], renderMode;
    struct SpiralParticleFrame frame;
};

struct SpiralBurst {
    u8 unknown0[0x1c];
    struct SceneVec origin;
    u8 collider[0x54];
    struct SpiralParticle particles[8];
    void *resource;
    u8 unknown1fc[6];
    u16 timer;
};
static inline s32 SpiralBurst_ScaleOffset(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

#endif
