#ifndef GUARD_POOLED_MOTION_H
#define GUARD_POOLED_MOTION_H
#include "contact_swarm.h"
struct MotionPoolSlot;
/* Tentative configurable motion pool, 080454C0–08045A20. */
struct MotionPoolParams {
    u16 firstFrame, endFrame, trackingFrames;
    s8 direction;
    u8 turnSpeed;
    s32 speed;
    u32 targetId;
    s32 (*callback)(struct MotionPoolSlot *, s32);
    u32 contactMode, contactFlags, resourceId, cost, option;
    struct ContactPosition bounds, offset;
    u32 graphicResource, frame;
    s32 tile;
};
struct MotionPoolSlot {
    u32 frame, flags;
    u8 unknown8[8];
    struct ContactPosition velocity;
    struct MotionPoolParams params;
    struct ContactSwarmGraphics graphics;
    struct ContactSwarmSprite sprite;
    struct ContactCollider collider;
    const struct ContactPosition *target;
    u8 unknownFC[0x40];
};
struct MotionPool {
    u8 task[0x18];
    u32 activeMask;
    struct MotionPoolSlot slots[16];
};
void MotionPool_FinishOnResourceContact(void *, void *, struct MotionPoolSlot *);
void MotionPool_InitializeSlot(struct MotionPool *, struct MotionPoolSlot *, s32);
void MotionPool_HideSlot(struct MotionPool *, struct MotionPoolSlot *, s32);
s32 MotionPool_Update(struct MotionPool *);
s32 MotionPool_Destroy(struct MotionPool *);
s32 MotionPool_Initialize(struct MotionPool *);
struct MotionPool *MotionPool_Create(void);
struct MotionPoolSlot *MotionPool_FindFreeSlot(struct MotionPool *, s32 *);
s32 MotionPool_Spawn(const struct MotionPoolParams *, const struct ContactPosition *);
void MotionPool_InitializeParams(struct MotionPoolParams *);
void MotionPool_SetLifetimeAndVelocity(struct MotionPoolParams *, u16, u16, u8, s32);
void MotionPool_SetTracking(struct MotionPoolParams *, u16, u32, u8);
void MotionPool_SetContact(struct MotionPoolParams *, u32, u32, u32, u32, u32);
void MotionPool_SetBounds(struct MotionPoolParams *, u16, u16, u16);
void MotionPool_SetOffset(struct MotionPoolParams *, u16, u16, u16);
void MotionPool_SetGraphics(struct MotionPoolParams *, u32, u32, s32);
void MotionPool_SetCallback(struct MotionPoolParams *, s32 (*)(struct MotionPoolSlot *, s32));
void MotionPool_AdjustHeading(struct MotionPoolSlot *);
s32 MotionPool_UpdateUntilDeadline(struct MotionPoolSlot *, s32);
#endif
