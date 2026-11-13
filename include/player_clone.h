#ifndef GUARD_PLAYER_CLONE_H
#define GUARD_PLAYER_CLONE_H
#include "scene_input.h"
#include "contact_collider.h"

struct PlayerCloneSlot;
typedef s32 (*CloneUpdateCallback)(struct PlayerCloneSlot *, s32);
struct CloneCallback {
    CloneUpdateCallback callback;
    u16 attack, element;
};

/* Reset the base animation and start its idle keyframe. */
#define PLAYER_CLONE_START_IDLE(index_, base_, sprite_, resource_)                                 \
    do {                                                                                           \
        (index_) = (base_);                                                                        \
        Anim_LoadKeyframe((sprite_), (resource_), 0, 1);                                           \
    } while (0)

struct PlayerCloneConfiguration {
    u16 flags, lifetime;
    u8 unknown4[2];
    s8 angle;
    u8 variant;
    s32 speed;
    struct CloneCallback operation;
    s16 primaryPower;
    u16 unknown16;
    s16 secondaryCollider[3];
    u16 unknown1E;
};

struct PlayerCloneSlot {
    u32 timer;
    u8 unknown04[8];
    struct ContactPosition velocity, position, previousPosition, targetPosition;
    struct ContactCollider contact, collider, area, playerAttack;
    u8 unknown17C[0x1C];
    struct PlayerCloneConfiguration configuration;
    struct SceneInputWidget animation;
    u8 unknown218[0x60];
    struct SceneInputResource resource;
    u8 shadow[0x6C];
    u32 flags, behaviorPhase;
    u8 unknown30C[0x10];
    s32 animationIndex;
    u8 foldedFacing, facing, mirrored, particleCount;
    s32 speed;
    u32 index;
    u8 unknown32C[4];
    s16 paletteTimer;
    s16 attackTimer;
    s16 hitPending, hitStrength, hitEffect, hitDuration;
    u32 debris;
    u16 blinkTimer, unknown342;
};

struct PlayerCloneManager {
    u8 task[0x18];
    u32 activeSlots;
    struct PlayerCloneSlot slots[3];
    u32 count;
    s16 palette1[16], palette2[16];
};
typedef char PlayerCloneSlotSizeCheck[sizeof(struct PlayerCloneSlot) == 0x344 ? 1 : -1];
u32 PlayerClone_GetActiveCount(void);
u32 PlayerClone_IsSlotActive(struct PlayerCloneManager *pool, s32 bit);
void PlayerClone_FoldFacing(struct PlayerCloneSlot *sprite);
s32 PlayerClone_SetAnimation(struct PlayerCloneSlot *s, s32 index, u32 frame, u32 mode);
void PlayerClone_OnBodyColliderContact(struct ContactCollider *contact,
                                       struct ContactCollider *other, struct PlayerCloneSlot *s);
void PlayerClone_OnPassiveColliderContact(void);
s32 PlayerClone_UpdateBehavior(struct PlayerCloneSlot *clone, s32 index);
void PlayerClone_InitSlot(void *unused, struct PlayerCloneSlot *s, u32 arg);
void PlayerClone_ReleaseSlot(struct PlayerCloneManager *a, struct PlayerCloneSlot *b, u32 n);
s32 PlayerClone_UpdatePool(struct PlayerCloneManager *pool);
u32 PlayerClone_DestroyPool(struct PlayerCloneManager *a);
s32 PlayerClone_InitializePool(struct PlayerCloneManager *pool);
void *PlayerClone_CreatePool(void);
struct PlayerCloneSlot *PlayerClone_FindFreeSlot(struct PlayerCloneManager *a, s32 *out);
s32 sub_08201160(struct PlayerCloneConfiguration *resource, struct ContactPosition *pos);
void PlayerClone_SetBehaviorCallback(struct PlayerCloneConfiguration *configuration,
                                     CloneUpdateCallback callback);
void PlayerClone_InitConfig(struct PlayerCloneConfiguration *p);
void PlayerClone_SetLifetimeAndBehavior(struct PlayerCloneConfiguration *p, u16 a, u32 b, u8 c);
void PlayerClone_SetColliderExtents(struct PlayerCloneConfiguration *p, u32 a, u32 b, u32 c, u32 d,
                                    u32 e, u32 f);
void PlayerClone_BeginActiveReactions(void);

struct ContactPosition *Player_GetEffectivePosition(u32);
struct ContactPosition *Ratatosk_GetPosition(void);
void Anim_SelectPaletteBank(void *, u32);
void ScaledShadow_Enable(void *);
void ScaledShadow_Disable(void *);
void ScaledShadow_InitializeDefault(void *, void *);
u32 ActorDebris_Create(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, u32, u32, s32);
void Collider_SetBodyProperties(void *, s32, u32, u32);

struct PlayerCloneColliderVector {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
};

struct CloneAreaWork {
    struct ContactCollider *collider;
    u8 *facing;
};

struct CloneColliderHeader {
    void *next;
    u16 unknown4;
    u16 flags;
};

struct CloneProjectionVectors {
    struct ContactPosition delta, position;
};

#endif
