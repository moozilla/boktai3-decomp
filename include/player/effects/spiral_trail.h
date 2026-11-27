#ifndef GUARD_SPIRAL_TRAIL_H
#define GUARD_SPIRAL_TRAIL_H

#include "engine/task.h"
#include "player/effects/spiral_trail_sprite.h"

struct Player;

/* The emitted orbit particles share the same position and origin as their update callback. */
struct SpiralTrailParticle {
    u32 flags;
    u8 unknown4[0xB], priority;
    u8 attribute, unknown11[7];
    struct ContactPosition position;
    u8 unknown20[8];
    u8 active, frame, timer, speed, radius, angle, life, unknown2F;
    struct ContactPosition origin;
    void (*update)(struct SpiralTrailParticle *, void *);
};

struct SpiralTrailEmitter {
    u8 next, delay, active, unknown3;
    void *graphics;
    struct SpiralTrailParticle particles[8];
};

struct SpiralTrailTask {
    struct Task task;
    struct Player *parent;
    struct ContactPosition offset;
    u8 kind, parentMode;
    u16 timer, emissionLimit, fadeLimit;
    struct SpiralTrailSprite sprite;
    struct SpiralTrailEmitter emitter;
    u8 mode;
};

typedef char SpiralTrailParticleSizeCheck[sizeof(struct SpiralTrailParticle) == 0x3C ? 1 : -1];
typedef char SpiralTrailTaskSizeCheck[sizeof(struct SpiralTrailTask) == 0x288 ? 1 : -1];

void AttachmentSprite_Setup(struct SpiralTrailTask *, const struct ContactPosition *);
void SpiralTrail_AdvanceFrame(struct SpiralTrailSprite *);
void SpiralTrail_RewindFrame(struct SpiralTrailSprite *);
void SpiralTrail_EmitOrbitParticle(struct SpiralTrailTask *, const struct ContactPosition *);
void SpiralTrail_UpdateOrbitParticle(struct SpiralTrailParticle *, void *);
void SpiralTrail_UpdateSlots(struct SpiralTrailTask *);
void SpiralTrail_RemoveParticles(struct SpiralTrailTask *);
void SpiralTrail_RemoveSprite(struct SpiralTrailTask *);
void sub_08236DC4(struct SpiralTrailTask *);
void sub_08236F48(struct SpiralTrailTask *);

#endif
