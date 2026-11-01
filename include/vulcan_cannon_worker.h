#ifndef GUARD_VULCAN_CANNON_WORKER_H
#define GUARD_VULCAN_CANNON_WORKER_H
#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"
/* Layout observed in initialization, state callbacks and collision consumers. */
struct VulcanCannon {
    u8 pad0[0x18];
    s32 timer;
    u8 body[0x48];
    struct SceneVec initialPosition;
    union {
        struct DisplaySequenceSprite sprite;
        struct {
            u8 pad[0x1c];
            struct SceneVec position;
        } spatial;
    } display;
    u8 graphics[0x1c];
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
    return PlayerCue_StepSequenceEnded(&owner->display.sprite, &owner->sequence);
}

#endif
