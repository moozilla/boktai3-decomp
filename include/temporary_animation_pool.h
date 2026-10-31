#ifndef GUARD_TEMPORARY_ANIMATION_POOL_H
#define GUARD_TEMPORARY_ANIMATION_POOL_H
#include "projected_display.h"
/* Sixteen resource-backed animations with independently decremented lifetimes. */
struct TemporaryAnimationSlot {
    u8 active, unknown1;
    s16 lifetime;
    struct ProjectedDisplayEntry animation;
};
struct TemporaryAnimationPool {
    u8 task[0x18];
    struct ProjectedDisplayResource resource;
    struct TemporaryAnimationSlot slots[16];
};
/* 0x020000A8 */
extern struct TemporaryAnimationPool *gTemporaryAnimationPool;
s32 TemporaryAnimationPool_Update(struct TemporaryAnimationPool *);
s32 TemporaryAnimationPool_Destroy(struct TemporaryAnimationPool *);
s32 TemporaryAnimationPool_Initialize(struct TemporaryAnimationPool *);
struct TemporaryAnimationPool *TemporaryAnimationPool_Create(void);
struct TemporaryAnimationSlot *TemporaryAnimationPool_FindInactive(struct TemporaryAnimationPool *);
struct TemporaryAnimationSlot *TemporaryAnimationPool_CreateEntry(u32, u32, u32, s32, const void *);
#endif
