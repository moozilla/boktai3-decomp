#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: follows the existing Lun movement callback interpretation.

// Remaining differences: instruction ordering, address lifetimes, and registers.
#include "scene_actor_reset.h"
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
extern u8 *gUnk_02000580[];
s32 Mod(s32, s32);
void SparkParticle_Spawn(void *, u32, void *, s32, s32, s32, s32, s32);
void Lun_ResumeAfterLiftPenalty(void);
void Lun_UpdateWaitingAnimation(void);
void Lun_BeginMoveToPosition(void *, void *);
void SceneActorControl_ResetCollisionFlags(void *);
struct Flags {
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
static inline u16 SetFlag(u16 *p, u32 mask)
{
    return *p = mask | *p;
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
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
static inline s32 Quarter(s32 v)
{
    if (v >= 0)
        return v >> 2;
    return -((-v) >> 2);
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
    a->motion.x += x;
    a->motion.z += z;
    *(s16 *)((u8 *)a + 0x318) += x;
    *(s16 *)((u8 *)a + 0x31C) += z;
    if (x == 0 && z == 0)
        return 1;
    return 0;
}
// confidence=low: eases Lun to its movement target, then selects the next waiting or pursuit state.
static inline u32 Arg1(u32 k_, struct SceneActorRegistryResetView *a)
{
    return a->flags2D4 & k_;
}

inline static u16 SetFlag_1(struct SceneActorRegistryResetView *a_, u32 mask)
{
    u16 *p = &a_->flags2C0;
    return *p = mask | (*p);
}

void Lun_ApproachMovementTarget(struct SceneActorRegistryResetView *a, s32 frame)
{
    u8 *ctx = a->context;
    if (TakeFlag(&a->field2B2)) {
        SetFlag(&a->flags2D4, 32);
        if (a->flags2D4 & 8)
            Sound_PlaySong(0x12A);
        {
            struct ContactCollider *c = &a->colliderB;
            c->flags |= 4;
        }
        {
            struct ContactCollider *c = &a->colliderA;
            {
                typeof(c->flags) *q1 = &c->flags;
                *q1 |= 4;
            }
        }
        SetFlag_1(a, 0x800);
    }
    if ((Mod(frame, 12) == 0) && Arg1(8, a)) {
        struct ContactPackedVector range;
        range.x = 20;
        range.y = 20;
        range.z = 20;
        SparkParticle_Spawn(&a->position, (Rand() & 3) + 1, &range, a->direction + 128, 10, 6, 2,
                            2);
    }
    {
        u32 mask = 0x100000;
        a->flags2D0 |= mask;
    }
    if (Move(a)) {
        a->field2B0 = 1;
        ((struct Flags *)(&a->flags2D4))->bits &= ~0x120;
        {
            u32 *p = &a->flags2CC;
            u32 mask = ~0x80;
            *p &= mask;
        }
        {
            u32 mask = 0x100000;
            a->flags2D0 |= mask;
        }
        ctx[0x3E2] = 0;
        {
            u16 k1 = 8;
            if (a->flags2D4 & k1) {
                SetState(a, 3, 0, 0);
                SetControl(a, Lun_ResumeAfterLiftPenalty, 20);
                ((struct Flags *)(&a->flags2D4))->bits &= ~8;
                a->field2C2 = 64;
            } else if (*((u16 *)(ctx + 0x444))) {
                SetFlag(&a->flags2D4, 0x100);
                *((u16 *)(ctx + 0x444)) = *((u16 *)(ctx + 0x442));
                Lun_BeginMoveToPosition(a, gUnk_02000580[0] + 0x30);
                SetState(a, 0, 2, 1);
                SceneActorControl_ResetCollisionFlags(a);
            } else {
                a->direction = a->fieldE9;
                SetControl(a, Lun_UpdateWaitingAnimation, 10);
                SetState(a, 0, 1, 1);
                a->direction = a->fieldE9;
                a->field2C2 = 64;
                SceneActorControl_ResetCollisionFlags(a);
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Lun_ApproachMovementTarget);
#endif
