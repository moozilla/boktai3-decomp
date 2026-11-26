#ifndef GUARD_SPIRAL_BURST_H
#define GUARD_SPIRAL_BURST_H
#include "engine/engine.h"
#include "engine/video/sprite_drawable_api.h"

/* Each burst owns eight 0x30-byte particles. The first 0x28 bytes are the
 * SpriteDrawable renderer record; the final eight bytes hold orbit parameters.
 * The existing frame/motion subdivision of that prefix remains provisional. */
struct SpiralParticleMotion {
    struct ContactPosition position;
    struct SpriteDrawable *previous, *next;
    u8 angle, riseRate, riseDelay, animationFrame, angularSpeed, lifetime;
    u16 radius;
};

struct SpiralParticleFrame {
    s8 depthOffset;
    u8 listIndex, unknown12[2];
    u32 attributes;
    struct SpiralParticleMotion motion;
};

struct SpiralParticle {
    u32 flags;
    u8 unknown4[11], priority;
    struct SpiralParticleFrame frame;
};

struct SpiralBurst {
    u8 unknown0[0x1c];
    struct ContactPosition origin;
    u8 collider[0x54];
    struct SpiralParticle particles[8];
    const struct SpriteEffectTileResource *resource;
    u8 unknown1fc[6];
    u16 timer;
};
static inline s32 SpiralBurst_ScaleOffset(s32 product)
{
    return product >= 0 ? product >> 12 : -((-product) >> 12);
}

#endif
