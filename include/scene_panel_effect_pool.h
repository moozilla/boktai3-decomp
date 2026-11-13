#ifndef GUARD_SCENE_PANEL_EFFECT_POOL_H
#define GUARD_SCENE_PANEL_EFFECT_POOL_H
#include "engine.h"
#include "contact_swarm_sprite.h"
#include "display_sequence.h"
#include "contact_collider.h"
#include "player_cue_sequence.h"

struct ScenePanelEffect;
struct ScenePanelOwnedEffect {
    void *update, *onComplete;
    u8 animationData[0x1C];
    struct ContactSwarmSprite sprite;     /* 0x24 */
    struct DisplaySequenceState sequence; /* 0x50 */
    u8 shadow[0x6C]; /* 0x60: shadow state, extent bounded by attack collider */
    struct ContactCollider attackCollider, bodyCollider; /* 0xCC, 0x120 */
    struct ContactPosition *anchor;                      /* 0x174 */
    struct ContactPosition motion;                       /* 0x178 */
    u8 state, age;
    s16 unknown182;
    struct ScenePanelEffect *owner;
};
struct ScenePanelSmallEffect {
    void *update, *onComplete;
    u8 unknown8[0x64];
};
struct ScenePanelEffectPool {
    struct ScenePanelSmallEffect small[24];
    u32 smallMask;
    struct ScenePanelOwnedEffect large[48];
    u32 largeMask[2];
    void *sequenceResource;
};
struct ScenePanelEffect {
    struct Task task;
    struct ScenePanelEffectPool pool;
    u16 impactVariant;
    s16 contactHundredths;
};
/* Packed stores preserve the unused upper halfword of a projected vector. */
struct ScenePanelEffectDelta {
    s32 x : 16, y : 16, z : 16;
};

u32 ScenePanelEffect_UpdateTask(struct ScenePanelEffect *effect);
u32 ScenePanelEffect_DestroyTask(struct ScenePanelEffect *effect);
struct ScenePanelEffect *ScenePanelEffect_Create(u32 impactVariant, u32 contactHundredths);
void ScenePanelEffect_OnContact(u32 first, u32 second, struct ScenePanelOwnedEffect *slot);
void ScenePanelEffect_CreatePositionEffect(const struct ContactPosition *position,
                                           struct ScenePanelEffect *effect);
void Nidhoggr_UpdateOwnedSeekingEffect(struct ScenePanelOwnedEffect *slot);
void Nidhoggr_CreateOwnedSeekingEffect(struct ScenePanelEffect *owner,
                                       struct ContactPosition *anchor,
                                       struct ContactPosition *offset);
#endif
