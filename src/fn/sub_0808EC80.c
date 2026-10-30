#include "global.h"

#include "scene_actor_chase.h"
// confidence=low: motion converges to saved displacement before choosing idle or the next Slime control.
void Slime_RollMoveFlag(void);
struct SlimeSequence {
    u8 unknown0[14];
    u16 timer;
};
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
static inline u8 TestContext(u8 *p, u32 mask)
{
    if (*(u16 *)(p + 0x40A) & mask)
        return 1;
    return 0;
}

static inline u32 Slime_IsReactionComplete(struct SceneActorRegistryResetView *actor)
{
    u32 complete = actor->field2B3;
    return complete;
}

void Slime_ConvergeReactionMotion(struct SceneActorRegistryResetView *actor)
{
    u8 *context = actor->context;
    struct SlimeSequence *sequence = (void *)((u8 *)actor->drawable + 0x48);
    s32 x;
    s32 z;
    s16 *priorX;
    s16 *priorZ;
    u32 *actorFlags;
    u32 *t;
    if (SceneActorReaction_TakePrepared(actor)) {
        actor->secondaryMotion.x = Quarter(actor->secondaryMotion.x);
        actor->secondaryMotion.z = Quarter(actor->secondaryMotion.z);
        SceneActorAnimation_SelectProfileEntry((void *)actor, 3);
        {
            u16 *flags = &actor->flags2D4;
            u32 mask = ~1;
            *flags = mask & *flags;
        }
        actor->field2B3 = 0;
    }
    {
        u32 mask = 0x2000;
        t = &actor->flags2CC;
        *t |= mask;
    }
    {
        u32 mask = 0x10000;
        actor->flags2D0 |= mask;
    }
    {
        u32 offset = 0x310;
        s32 current = *(s16 *)((u8 *)actor + offset);
        s16 *field = (s16 *)((u8 *)actor + 0x318);
        s32 difference = current - *field;
        actorFlags = t;
        priorX = field;
        x = Quarter(difference);
    }
    {
        u32 offset = 0x314;
        s32 current = *(s16 *)((u8 *)actor + offset);
        s16 *field = (s16 *)((u8 *)actor + 0x31C);
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
    {
        u32 rest = AtRest(x, z);
        if (!rest)
            sequence->timer = rest;
        else if (Slime_IsReactionComplete(actor)) {
            u8 *field = &actor->field2B0;
            u32 ready = 1;

            *field = ready;
            {
                u16 *flags = &actor->flags2D4;
                u32 mask = ready - 34;
                *flags = mask & *flags;
            }
            {
                u32 mask = ~0x80;
                *actorFlags = mask & *actorFlags;
            }
            if (actor->health < *(u16 *)(context + 0x3DA) || TestContext(context, 8)) {
                Chase_SetCtl(actor, SceneActorControl_OnIdle, 0x10);
                Chase_SetBehavior(actor, 2);
                {
                    u16 *flags = &actor->flags2D6;
                    u32 mask = ~0x980;
                    *flags = mask & *flags;
                }
            } else {
                Chase_SetCtl(actor, Slime_RollMoveFlag, 0);
                Chase_SetState(actor, 0, 0, 0);
            }
        }
    }
}
