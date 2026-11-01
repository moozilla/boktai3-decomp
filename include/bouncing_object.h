#ifndef GUARD_BOUNCING_OBJECT_H
#define GUARD_BOUNCING_OBJECT_H
#include "contact_swarm.h"
#include "ram.h"
/* Separate twelve-slot family; not the preceding anchored-particle pool. */
struct BouncingObject {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactCollider bodyCollider, removalCollider;
    u8 registration[0x40];
    struct ContactPosition velocity, origin;
    u16 flags;
    u8 state;
    s8 index;
    u8 enter, collisionDelay;
    u16 timer, mapCell, spawnParameterA, spawnParameterB;
    u8 animationDelay, animation;
    u32 colliderConfig;
    u16 initialTimer, stateFrame;
};
struct BouncingPool {
    u8 task[0x18];
    u8 graphics[0x1C];
    void *sequenceResource;
    struct BouncingObject slots[12];
    u32 used, stopping;
};
struct BouncingContact {
    u8 unknown0[0xC];
    struct ContactPosition position;
    u8 unknown14[0x38 - 0x14];
    u32 flags;
};
extern const u8 gBouncingObjectBlinkDelays[4];
typedef void (*BouncingObjectCallback)(struct BouncingPool *, struct BouncingObject *);
extern BouncingObjectCallback const gBouncingObjectStateCallbacks[3];
extern u32 gUnk_02000560;
struct BouncingPool *BouncingObject_EnsurePool(void);
void Shadow_Initialize(void *, struct ContactPosition *, s32);
void BouncingObject_InitializeColliders(struct BouncingObject *, s16, s16);
void Shadow_Destroy(void *);
/* 0x0200014C */
extern struct BouncingPool *gBouncingPool;
struct BouncingObject *BouncingObject_Allocate(struct BouncingPool *);
void BouncingObject_ApplyContactImpulse(struct BouncingContact *, struct BouncingContact *,
                                        struct BouncingObject *);
void BouncingObject_HandleTimerContact(struct BouncingContact *, void *, struct BouncingObject *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
s32 Math_Atan2(s32, s32);
#endif
