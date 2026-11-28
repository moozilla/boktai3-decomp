#ifndef GUARD_RADIAL_BURST_H
#define GUARD_RADIAL_BURST_H

#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/video/sprite_drawable_api.h"
#include "effects/bursts/spiral_burst.h"

/* One of the emitter's twelve particles. The fields past the timer are used differently by the
 * three particle kinds (spray, sweep and orbit), chosen by the update callback. */
struct RadialBurstParticle {
    struct SpriteDrawable drawable;
    u8 active, tile, angle;
    u8 angularSpeed;
    u16 timer;
    u16 speed;
    u16 radius;
    u16 orbitRadius;
    struct ContactPosition *orbitOrigin;
    union {
        struct ContactPosition origin;   /* sweep particles */
        struct ContactPosition velocity; /* spray particles */
    } vector;
    void *update;
};

struct RadialBurstFade {
    u32 flags;
    u8 unk_04[0x18];
    struct ContactPosition position;
    u8 unk_24[0x24];
    const u16 *source;
    u16 colors[16];
    u8 active, alpha, fadingOut;
    u8 unk_6F[3];
    u16 ticks;
    struct RadialBurst *owner;
};

struct RadialBurst {
    struct Task task;
    u8 kind, started;
    u8 unk_1A[2];
    struct Player *player;
    u8 unk_20[0xA0];
    struct RadialBurstFade fade;
    u8 nextParticle, emitDelay, particlesActive;
    u8 unk_13B;
    const struct SpriteEffectTileResource *graphics;
    struct RadialBurstParticle particles[12];
};

extern const s16 gUnk_086149C4[];

#endif
