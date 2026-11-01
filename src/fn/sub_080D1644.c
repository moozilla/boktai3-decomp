#include "global.h"
#include "scene_actor_chase.h"
void Mummy_ResetMotionState(void *);
void Mummy_ResumeSavedRouteAfterAnimation(void *);
static inline void StoreHalf(struct SceneActorRegistryResetView *a, u32 v)
{
    u32 value = v;
    *(u16 *)((u8 *)a + 0x1EA) = value;
}
static inline void SetHalf(struct SceneActorRegistryResetView *a, u16 mask)
{
    a->flags2D4 = mask | a->flags2D4;
}

// confidence=low: waits for the recovery timer before restoring route animation.
#define Mummy_UpdateRecoveryWait sub_080D1644
void Mummy_UpdateRecoveryWait(struct SceneActorRegistryResetView *a)
{
    u8 *group = (u8 *)&a->group.common;
    u16 *speed;
    if (SceneActorReaction_TakePrepared(a)) {
        SceneActorAttachment_ReplaceResponseSlot(a, 0, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(a, 1, 127, 0);
        SceneActorAttachment_ReplaceResponseSlot(a, 3, 127, 0);
        SetHalf(a, 16);
    }
    SceneActorAnimation_SelectDirectionalEntry((void *)a, 1);
    a->field2C2 = Div(64, 6);
    speed = &a->field2C2;
    if (!*(u16 *)(group + 0x1F0)) {
        a->field2B0 = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)a, 6);
        Mummy_ResetMotionState(a);
        StoreHalf(a, 64);
        Chase_SetState(a, 3, 0, 0);
        Chase_SetCtl(a, (void *)Mummy_ResumeSavedRouteAfterAnimation, 0x13);
        {
            u16 *p = &a->flags2D4;
            s32 mask = ~0x30;
            *p &= mask;
        }
        {
            u32 mask = 16;
            a->flags2CC |= mask;
        }
        *speed = 64;
    }
}
