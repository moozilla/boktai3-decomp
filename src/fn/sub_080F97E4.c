// MATCHING_CONCESSION: register-pinning
#include "scene_actor_control_phase.h"
// confidence=low: converges saved motion, then enables the axe recovery behavior.

void SceneActorAnimation_SelectProfileEntry(struct SceneActorAnimationView *, s32);
void Axe_ClearForcedMovement(void *);
void Axe_ResetMovementFlags(void *);
static inline s32 Quarter(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}
static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}

static inline void SetFlags(struct SceneActorRegistryResetView *a, u32 m)
{
    asm("" : "+r"(m));
    a->flags2D4 = m | a->flags2D4;
}
static inline void SetForced(struct SceneActorRegistryResetView *a, u32 m)
{
    a->flags2D0 = m | a->flags2D0;
}
static inline void SetContext(u8 *p, u32 m)
{
    *(u32 *)(p + 0xE4C) = m | *(u32 *)(p + 0xE4C);
}
static inline void RestoreHistory(struct SceneActorRegistryResetView *a, u16 v)
{
    *(u16 *)((u8 *)a + 0x418) = v;
}
static inline void SetState(struct SceneActorRegistryResetView *a, u8 s, u8 p, u8 b)
{
    a->state2A8 = s;
    a->phase2A9 = p;
    a->behavior2AA = b;
    a->counter2AC = 0;
    a->field2B1 = 1;
}
void Axe_ConvergeRecoveryMotion(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    s32 x, z;
    s16 *priorX, *priorZ;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        SetFlags(actor, 0x20);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        Axe_ClearForcedMovement(actor);
        Axe_ResetMovementFlags(actor);
    }
    {
        s32 current = *(s16 *)((u8 *)actor + 0x310);
        s16 *field = (void *)((u8 *)actor + 0x318);
        s32 difference = current - *field;
        priorX = field;
        x = Quarter(difference);
    }
    {
        s32 current = *(s16 *)((u8 *)actor + 0x314);
        s16 *field = (void *)((u8 *)actor + 0x31C);
        s32 difference = current - *field;
        priorZ = field;
        z = Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        actor->field2B0 = 1;
        {
            struct SignedFlag {
                s32 bits : 16;
            };
            struct SignedFlag *f = (void *)&actor->flags2D4;
            s32 m = ~0x20;
            m &= (u16)f->bits;
            f->bits = m;
        }
        {
            u32 *f = &actor->flags2CC;
            s32 m = ~0x80;
            *f &= m;
        }
        SetForced(actor, 0x100000);
        Axe_ResetMovementFlags(actor);
        SetContext(context, 0x10);
        RestoreHistory(actor, *(u16 *)((u8 *)actor + 0x416));
        SetState(actor, 0, 2, 9);
    }
}
