#ifndef GUARD_CONTACT_BURST_H
#define GUARD_CONTACT_BURST_H
#include "transient_sprite_effects.h"
#include "engine_globals.h"
/* Tentative contact-triggered aura; source is a caller-owned world position. */
struct ContactBurst {
    u8 task[0x18];
    u8 enabled, previousEnabled, contactTimer, contactDirection;
    u16 frame, unknown1E;
    u8 fade, contactFrame, unknown22[2];
    const struct ContactPosition *world;
    struct ContactSwarmGraphics graphics;
    struct TransientEffectSprite sprite;
    struct ContactCollider collider;
};
struct ContactBurst *ContactBurst_Create(const struct ContactPosition *);
s32 ContactBurst_Update(struct ContactBurst *);
s32 ContactBurst_Destroy(struct ContactBurst *);
s32 ContactBurst_Initialize(struct ContactBurst *, const struct ContactPosition *);
void ContactBurst_UpdateContactAnimation(struct ContactBurst *);
u32 ContactBurst_RandomParticleFrame(void);
void ContactBurst_SetEnabled(struct ContactBurst *, u32);
u32 ContactBurst_GetEnabled(struct ContactBurst *);
#endif
