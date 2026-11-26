#ifndef GUARD_BOSSES_NIDHOGGR_NIDHOGGR_DUEL_BURST_H
#define GUARD_BOSSES_NIDHOGGR_NIDHOGGR_DUEL_BURST_H
#include "effects/sprites/transient_sprite_effects.h"
#include "engine/engine_globals.h"
/* Tentative contact-triggered aura; source is a caller-owned world position. */
struct ContactBurst {
    struct Task task;
    u8 enabled, previousEnabled, contactTimer, contactDirection;
    u16 frame, unknown1E;
    u8 fade, contactFrame, unknown22[2];
    const struct ContactPosition *world;
    struct ContactSwarmGraphics graphics;
    struct TransientEffectSprite sprite;
    struct ContactCollider collider;
};
struct ContactBurst *NidhoggrDuelBurst_Create(const struct ContactPosition *);
s32 NidhoggrDuelBurst_Update(struct ContactBurst *);
s32 NidhoggrDuelBurst_Destroy(struct ContactBurst *);
s32 NidhoggrDuelBurst_Initialize(struct ContactBurst *, const struct ContactPosition *);
void NidhoggrDuelBurst_UpdateContactAnimation(struct ContactBurst *);
u32 NidhoggrDuelBurst_RandomParticleFrame(void);
void NidhoggrDuelBurst_SetEnabled(struct ContactBurst *, u32);
u32 NidhoggrDuelBurst_GetEnabled(struct ContactBurst *);
#endif
