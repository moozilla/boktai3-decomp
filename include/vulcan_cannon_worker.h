#ifndef GUARD_VULCAN_CANNON_WORKER_H
#define GUARD_VULCAN_CANNON_WORKER_H
#include "global.h"
#include "display_animation.h"
#include "display_sequence_sprite.h"
#include "task.h"
/* Layout observed in initialization, state callbacks and collision consumers. */
struct VulcanCannon {
    struct Task task;
    s32 timer;
    u8 body[0x48];
    struct ContactPosition initialPosition;
    struct DisplaySequenceSprite display;
    struct SpriteRenderResource graphics;
    u8 collider[0x54];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s32 range;
    s16 attack, element, power, hp, damage, damaged, unknown12c, dead, flash;
    s8 facing;
    u8 allowTurn;
    s16 turnDelay, speed, shotCount, targetVisible, previousVisible;
    s16 burstDelay, burstCount, aimTimer, maxHp, bodyFlags, bodyPower;
    s16 hudStyle, unknown14c, hudDuration, defeatId, unknown152, inert;
    u16 unknown156;
    void *owner;
    u32 hitFlags, colliderHitFlags, args[3];
    void *delayScript;
    u32 deathScript, terrainEnabled;
    void *resource, *attachment;
    u8 terrain[0x10];
    s16 delayCount, delayIndex, delays[8];
};
static inline u32 VulcanCannon_StepAnimation(struct VulcanCannon *owner)
{
    return DisplaySequence_StepSprite(&owner->display, &owner->sequence);
}

/* Defeat particles and explosions originate half a world unit above the sprite. */
static inline void VulcanCannon_GetEffectPosition(struct ContactPosition *out,
                                                  const struct VulcanCannon *owner)
{
    *out = owner->display.position;
    out->y += 128;
}

#endif
