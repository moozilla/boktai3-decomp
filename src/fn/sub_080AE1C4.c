#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 83.5%
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: converges saved motion and restores the route after a reaction timer.
// Remaining differences: reset-address lifetimes, saved registers, and collision-control scheduling; delaying maskTwo and spilling group reduced structural differences.
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
void Vorn_SendMessage(void *);
u8 Vorn_HasActiveResponse(void *);
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
static inline s32 Quarter(s32 n)
{
    return n >= 0 ? n >> 2 : -((-n) >> 2);
}
static inline s32 Divide32(s32 n)
{
    return n >= 0 ? n >> 5 : -((-n) >> 5);
}
static inline s32 Clamp(s32 n)
{
    if (n > 200)
        return 200;
    if (n < -200)
        return -200;
    return n;
}
static inline u8 Rest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline void ClearMotion(u16 *flags, s32 mask)
{
    *flags &= mask;
}
static inline void SetMotion(struct SceneActorReactionMotion *m, u32 mask)
{
    m->flags |= mask;
}
static inline u8 Test(u16 *p, u32 mask)
{
    if (*p & mask)
        return 1;
    return 0;
}
inline static u32 Arg1(u32 k_, struct SceneActorRegistryResetView *actor)
{
    return ((u8 *)actor->context) + k_;
}

void Vorn_UpdateKnockbackRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *volatile group = (u8 *)(&actor->group);
    s32 timer;
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    if (SceneActorReaction_TakePrepared(actor)) {
        u32 maskTwo;
        u32 *flags;
        u16 *motionFlags;
        struct SceneActorReactionMotion *motion;
        struct SceneActorRegistryControl *control;
        u32 *actionFlags;
        actor->body.direction = (((actor->body.direction + 32) >> 6) & 3) << 6;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 6);
        maskTwo = 2;
        flags = &actor->flags2D0;
        if (!((*flags) & maskTwo)) {
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0, 25);
            {
                *flags |= maskTwo;
            }
            Sound_PlaySong(0x1CC);
        }
        motionFlags = &actor->flags2D4;
        {
            u32 v = (*motionFlags) | maskTwo;
            v &= ~0x17A9;
            *motionFlags = v;
        }
        actor->field2C2 = 64;
        *((u16 *)(((u8 *)actor) + 0x414)) = 0;
        *((u16 *)(((u8 *)actor) + 0x418)) = 0;
        ClearMotion(motionFlags, ~0x380);
        motion = (void *)(&actor->tracking.target);
        {
            s32 mask = ~0x18;
            motion->flags &= mask;
        }
        control = &actor->group.control;
        actionFlags = &actor->flags2CC;
        if ((*actionFlags) & 0x200000) {
            *flags &= ~0x1000000;
            ClearMotion(motionFlags, ~0x200);
            control->mode = 0;
            control->counter = 0;
            control->timer = 0;
        }
        ClearMotion(motionFlags, ~0x400);
        *(((u8 *)actor) + 0x5DA) = 0xFF;
        {
            s32 mask = ~0x80;
            *actionFlags &= mask;
        }
        actor->field2C2 = 64;
        {
            s32 mask = ~0x18;
            motion->flags &= mask;
        }
        *flags &= ~0x1000000;
        {
            struct SceneActorRegistryControl *control = (void *)(group + 0x4C);
            {
                u16 k1 = 0x141C0;
                *actionFlags &= ~k1;
            }
            control->counter = 0;
            *((u16 *)(((u8 *)control) + 0x14)) = 0;
            control->timer = 0;
        }
        {
            u32 mask = 0x2000000;
            *actionFlags |= mask;
        }
        Vorn_SendMessage(actor);
    }
    {
        u32 value = 65;
        *((u16 *)(((u8 *)actor) + 0x1EA)) = value;
    }
    timer = (s16)actor->fieldAA;
    if (((u32)(timer - 30)) <= 89) {
        s32 frame = Mod(timer, 30);
        s32 threshold = 15 - Div(timer, 10);
        if (frame < threshold) {
            u32 mask = 32;
            actor->flags2D6 |= mask;
        }
    }
    {
        s32 current = *((s16 *)(((u8 *)actor) + 0x310));
        s16 *p = (void *)(((u8 *)actor) + 0x318);
        s32 delta = current - (*p);
        priorX = p;
        x = Quarter(delta);
    }
    {
        s32 current = *((s16 *)(((u8 *)actor) + 0x314));
        s16 *p = (void *)(((u8 *)actor) + 0x31C);
        s32 delta = current - (*p);
        priorZ = p;
        z = Quarter(delta);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (Rest(x, z) && (timer <= 0)) {
        struct ContactCollider *collider;
        u16 *motionFlags;
        actor->fieldAA = 0;
        actor->field2B6 = (Divide32(actor->body.direction) + 7) & 7;
        Chase_SetState(actor, 3, 1, 0);
        Chase_SetCtl(actor, *((void **)(group + 0x20C)), 8);
        {
            s8 mask = ~2;
            actor->flags2D0 &= mask;
        }
        motionFlags = &actor->flags2D4;
        {
            s32 mask = ~0x3A;
            *motionFlags &= mask;
        }
        Chase_SetState(actor, 3, 1, 0);
        actor->body.direction = ((actor->body.direction + 32) >> 6) << 6;
        SceneBody_StartRoute(&actor->body.id, group + 0x74, group[0x84], group[0x85], group[0x86]);
        collider = &actor->colliderA;
        if (Test((u16 *)(((u8 *)actor) + 0x2C0), 64))
            collider->flags &= 0xFEFF;
        else if ((*motionFlags) & 0x2016)
            collider->flags &= 0xFEFF;
        else
            collider->flags |= 0x100;
        {
            u16 *f = (u16 *)Arg1(0x6D4, actor);
            s32 mask = ~0x1E;
            *f &= mask;
        }
        if (Vorn_HasActiveResponse(actor->context)) {
            SetMotion((void *)(&actor->tracking.target), 8);
            {
                u32 mask = 0x80;
                actor->flags2CC |= mask;
            }
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Vorn_UpdateKnockbackRecovery);
#endif
