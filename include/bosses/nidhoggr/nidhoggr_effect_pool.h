#ifndef GUARD_NIDHOGGR_EFFECT_POOL_H
#define GUARD_NIDHOGGR_EFFECT_POOL_H
#include "engine/engine.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "engine/video/display_sequence.h"
#include "scene/collision/contact_collider.h"
#include "player/cutscene/player_cue_sequence.h"

struct NidhoggrEffect;
struct NidhoggrOwnedEffect {
    void *update, *onComplete;
    u8 animationData[0x1C];
    struct ContactSwarmSprite sprite;     /* 0x24 */
    struct DisplaySequenceState sequence; /* 0x50 */
    u8 shadow[0x6C]; /* 0x60: shadow state, extent bounded by attack collider */
    struct ContactCollider attackCollider, bodyCollider; /* 0xCC, 0x120 */
    struct ContactPosition *anchor;                      /* 0x174 */
    struct ContactPosition motion;                       /* 0x178 */
    u8 state, age;
    s16 referenceCoordinate; /* starting X for lateral motion, height for falling effects */
    struct NidhoggrEffect *owner;
};
struct NidhoggrSmallEffect {
    void *update, *onComplete;
    u8 unknown8[0x64];
};
struct NidhoggrEffectPool {
    struct NidhoggrSmallEffect small[24];
    u32 smallMask;
    struct NidhoggrOwnedEffect large[48];
    u32 largeMask[2];
    void *sequenceResource;
};
struct NidhoggrEffect {
    struct Task task;
    struct NidhoggrEffectPool pool;
    u16 impactVariant;
    s16 contactHundredths;
};
/* Packed stores preserve the unused upper halfword of a projected vector. */
struct NidhoggrEffectDelta {
    s32 x : 16, y : 16, z : 16;
};

u32 NidhoggrEffect_UpdateTask(struct NidhoggrEffect *effect);
u32 NidhoggrEffect_DestroyTask(struct NidhoggrEffect *effect);
struct NidhoggrEffect *NidhoggrEffect_Create(u32 impactVariant, u32 contactHundredths);
void NidhoggrEffect_OnContact(u32 first, u32 second, struct NidhoggrOwnedEffect *slot);
void NidhoggrEffect_CreatePositionEffect(const struct ContactPosition *position,
                                           struct NidhoggrEffect *effect);
void Nidhoggr_UpdateOwnedSeekingEffect(struct NidhoggrOwnedEffect *slot);
void Nidhoggr_CreateOwnedSeekingEffect(struct NidhoggrEffect *owner,
                                       struct ContactPosition *anchor,
                                       struct ContactPosition *offset);
#endif
