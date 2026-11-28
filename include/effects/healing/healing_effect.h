#ifndef GUARD_HEALING_EFFECT_H
#define GUARD_HEALING_EFFECT_H

#include "global.h"
#include "engine/task.h"
#include "engine/video/anim_sprite.h"
#include "engine/video/sprite_drawable_api.h"
#include "scene/collision/contact_collider.h"
#include "player/player.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"

/* A pooled screen sparkle (0x3C bytes). */
struct HealingParticle {
    struct SpriteDrawable drawable;
    u16 age, active;
    struct ContactPosition velocity;
    const struct SpriteEffectTileResource *resource;
    void (*update)(struct HealingParticle *);
};

/* The sun-bank restore sequence: an icon slides in, shakes and refills the life gauge. */
struct HealthRestoreEffect {
    struct Task task;
    struct Player *owner;
    struct AnimSprite icon;                /* 0x1C */
    struct ContactPosition targetPosition; /* 0x7C: screen position the icon ends at */
    struct ContactPosition startPosition;  /* 0x84: the owner's screen position */
    struct HealingParticle particles[8];   /* 0x8C */
    u16 frame, nextParticle, timer;        /* 0x26C */
    u8 unk_272[2];
    void (*state)(struct HealthRestoreEffect *); /* 0x274 */
};

struct HealthBurstSprite {
    u32 flags;
    u8 unk_04[0x18];
    struct ContactPosition position;
    u8 unk_24[8];
};

struct HealthBurstEffect {
    struct Task task;
    struct Player *owner;
    struct AnimSprite icon;                /* 0x1C */
    struct ContactPosition targetPosition; /* 0x7C */
    struct ContactPosition startPosition;  /* 0x84 */
    u8 unk_8C[8];
    struct ContactCollider collider;     /* 0x94 */
    struct HealthBurstSprite sprites[2]; /* 0xE8 */
    u8 spriteGraphics[0x1C];             /* 0x140 */
    u16 nextSprite;                      /* 0x15C */
    u8 spriteState[2];                   /* 0x15E */
    u16 floorHeight[2];                  /* 0x160 */
    u16 spriteTimer[2];                  /* 0x164 */
    struct HealingParticle particles[8]; /* 0x168 */
    u8 unk_348[4];
    u16 mode;                                  /* 0x34C */
    u16 timer;                                 /* 0x34E */
    void (*state)(struct HealthBurstEffect *); /* 0x350 */
};

/* One draw from the random table, as the healing effects use it. */
static inline s32 HealingEffect_Random(const u16 *table)
{
    s32 sample;

    gRandomIndex = (gRandomIndex + 1) & 1023;
    sample = table[gRandomIndex];
    return sample >> 3;
}

void FlickerParticle_UpdateMotion(struct HealingParticle *);
void FlickerParticle_UpdateVerticalMotion(struct HealingParticle *);
void FlickerParticle_UpdateStationary(struct HealingParticle *);
void FlickerParticle_UpdateSecondaryMotion(struct HealingParticle *);

void HealthRestoreEffect_StartParticleBurst(struct HealthRestoreEffect *);
void HealthRestoreEffect_InitRisingParticle(struct HealingParticle *);
void HealthRestoreEffect_InitSparkle(struct HealingParticle *);
void HealthRestoreEffect_UpdateParticles(struct HealthRestoreEffect *);
void HealthRestoreEffect_UpdateCompletion(struct HealthRestoreEffect *);
void HealthRestoreEffect_UpdateRestoration(struct HealthRestoreEffect *);
void HealthRestoreEffect_UpdateShake(struct HealthRestoreEffect *);
void sub_08234DC8(struct HealthRestoreEffect *);
u32 HealthRestoreEffect_OnUpdate(struct HealthRestoreEffect *);
s32 HealthRestoreEffect_OnDestroy(struct HealthRestoreEffect *);
void HealthRestoreEffect_InitAnimation(struct HealthRestoreEffect *);
void HealthRestoreEffect_InitParticlePool(struct HealthRestoreEffect *);
s32 HealthRestoreEffect_Init(struct HealthRestoreEffect *, struct Player *);
struct HealthRestoreEffect *HealthRestoreEffect_Create(struct Player *);

void FlickerParticle_StartSecondaryBurst(struct HealthBurstEffect *);
void FlickerParticle_UpdatePool(struct HealthBurstEffect *);
void HealthBurstEffect_UpdateFadeOut(struct HealthBurstEffect *);
void HealthBurstEffect_StartBurst(struct HealthBurstEffect *);
void HealthBurstEffect_UpdateBurstSprites(struct HealthBurstEffect *);
void HealthBurstEffect_UpdateBursts(struct HealthBurstEffect *);
void sub_082356E4(struct HealthBurstEffect *);
void sub_08235794(struct HealthBurstEffect *);
void sub_082358C0(struct HealthBurstEffect *);
u32 HealthBurstEffect_OnUpdate(struct HealthBurstEffect *);
s32 SceneEffectGroup_Remove(struct HealthBurstEffect *);
void sub_082359CC(struct HealthBurstEffect *);
void HealthBurstEffect_InitSprites(struct HealthBurstEffect *);
void HealthBurstEffect_InitParticlePool(struct HealthBurstEffect *);
void sub_08235B34(struct HealthBurstEffect *);
struct HealthBurstEffect *HealthBurstEffect_Create(struct Player *, u32);

#endif
