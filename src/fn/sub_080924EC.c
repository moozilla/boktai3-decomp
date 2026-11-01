#include "scene_actor_control_phase.h"
#include "player.h"
void SceneActorControl_ResetCollisionPhase(struct SceneActorRegistryResetView *);
void SceneActorControl_StartPositionCurve(struct SceneActorRegistryResetView *, struct SceneVec *);
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
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void Bee_DisableMovement(void *);
// confidence=low: eases saved motion before returning to idle.
void Bee_UpdateMovementStop(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    s32 x, z;
    s16 *priorX, *priorZ;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u16 *flags;
        {
            u32 value = 0x20;
            flags = &actor->flags2D4;
            value |= *flags;
            *flags = value;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        {
            u32 mask = ~0x100;
            *flags = mask & *flags;
        }
    }
    {
        u32 mask = 0x100000;
        actor->flags2D0 |= mask;
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
        Bee_DisableMovement(actor);
        SetPhase(actor, 0, 2, 0);
        {
            u32 mask = 8;
            u32 *flags = (void *)(body + 0x3D4);
            *flags |= mask;
        }
        body[0x42A] = 60;
    }
}
