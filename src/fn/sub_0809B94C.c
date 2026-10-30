#include "global.h"

#include "scene_actor_reset.h"
#include "engine.h"
#include "player.h"
#include "ram.h"
s32 Mod(s32, s32);
void SparkParticle_Spawn(void *, u32, void *, s32, s32, s32, s32, s32);
void Lun_ResumeAfterLiftPenalty(void);
void Lun_UpdateWaitingAnimation(void);
void Lun_BeginMoveToPosition(void *, void *);
void SceneActorControl_ResetCollisionFlags(void *);
struct LunFlagWord {
    s16 bits : 16;
};
static inline u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline s32 SetFlag(u16 mask, struct LunFlagWord *p)
{
    return p->bits = mask | p->bits;
}
static inline s32 ClearFlag(struct LunFlagWord *p, u32 mask)
{
    return p->bits = mask & p->bits;
}
static inline void SetControl(struct SceneActorRegistryResetView *a, void *callback, u32 kind)
{
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = callback;
    a->field2C8 = 0;
    {
        u32 *flags = &a->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    a->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u32 state, u32 phase,
                            u32 behavior)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
static inline u32 Rand(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 offset;
    const u16 *entry;
    gRandomIndex = index;
    offset = index * 2;
    entry = (const u16 *)(offset + (u32)RANDOM_TABLE_ADDR);
    return *entry;
}
static inline s32 Quarter(s32 v)
{
    if (v >= 0)
        v >>= 2;
    else
        v = -((-v) >> 2);
    return v;
}
static inline void SetMotionFlags(struct SceneActorRegistryResetView *a, u32 input)
{
    u32 mask = input;
    u32 *p = &a->flags2D0;
    *p |= mask;
}
static inline u8 Move(struct SceneActorRegistryResetView *a)
{
    s32 x = Quarter(*(s16 *)((u8 *)a + 0x310) - *(s16 *)((u8 *)a + 0x318));
    s32 z = Quarter(*(s16 *)((u8 *)a + 0x314) - *(s16 *)((u8 *)a + 0x31C));
    if (x > 200)
        x = 200;
    else if (x < -200)
        x = -200;
    if (z > 200)
        z = 200;
    else if (z < -200)
        z = -200;
    a->body.motion.x += x;
    a->body.motion.z += z;
    *(s16 *)((u8 *)a + 0x318) += x;
    *(s16 *)((u8 *)a + 0x31C) += z;
    if (x == 0 && z == 0)
        return 1;
    return 0;
}
// confidence=low: eases Lun to its movement target, then selects the next waiting or pursuit state.
static inline u32 TestMotionFlags(u32 mask, struct SceneActorRegistryResetView *a)
{
    return a->flags2D4 & mask;
}

inline static u16 SetAnimationFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    u16 *p = &actor->flags2C0;
    return *p = mask | (*p);
}

void Lun_ApproachMovementTarget(struct SceneActorRegistryResetView *a, s32 frame)
{
    u8 *ctx = a->context;
    if (TakeFlag(&a->field2B2)) {
        SetFlag(32, (struct LunFlagWord *)&a->flags2D4);
        if (a->flags2D4 & 8)
            Sound_PlaySong(0x12A);
        {
            struct ContactCollider *c = &a->colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &a->colliderA;
            c->flags |= 4;
        }
        SetAnimationFlags(a, 0x800);
    }
    if ((Mod(frame, 12) == 0) && TestMotionFlags(8, a)) {
        struct ContactPackedVector range;
        range.x = 20;
        range.y = 20;
        range.z = 20;
        SparkParticle_Spawn(&a->body.position, (Rand() & 3) + 1, &range, a->body.direction + 128, 10, 6, 2,
                            2);
    }
    SetMotionFlags(a, 0x100000);
    if (Move(a)) {
        a->field2B0 = 1;
        ClearFlag((struct LunFlagWord *)&a->flags2D4, ~0x120);
        {
            u32 *p = &a->flags2CC;
            u32 mask = ~0x80;
            *p &= mask;
        }
        SetMotionFlags(a, 0x100000);
        ctx[0x3E2] = 0;
        {
            u16 liftFlag = 8;
            if (a->flags2D4 & liftFlag) {
                SetState(a, 3, 0, 0);
                SetControl(a, Lun_ResumeAfterLiftPenalty, 20);
                ClearFlag((struct LunFlagWord *)&a->flags2D4, ~8);
                a->field2C2 = 64;
            } else if (*((u16 *)(ctx + 0x444))) {
                SetFlag(0x100, (struct LunFlagWord *)&a->flags2D4);
                *((u16 *)(ctx + 0x444)) = *((u16 *)(ctx + 0x442));
                Lun_BeginMoveToPosition(a, ((u8 **)gPlayers)[0] + 0x30);
                SetState(a, 0, 2, 1);
                SceneActorControl_ResetCollisionFlags(a);
            } else {
                a->body.direction = a->tracking.heading;
                SetControl(a, Lun_UpdateWaitingAnimation, 10);
                SetState(a, 0, 1, 1);
                a->body.direction = a->tracking.heading;
                a->field2C2 = 64;
                SceneActorControl_ResetCollisionFlags(a);
            }
        }
    }
}
