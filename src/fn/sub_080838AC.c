#include "scene_actor_control_motion.h"
static inline u32 ActorFlag(struct SceneActorRegistryResetView *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}
static inline u8 IsUnavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor &&
        (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) || actor->health <= 0))
        return 1;
    return 0;
}
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline s32 TruncateSine(s32 value, u32 shift)
{
    /* Both callers pass signed halfword samples; negation stays within s32. */
    return value >= 0 ? value >> shift : -((-value) >> shift);
}
void SceneActorControl_TickGroundBob(struct SceneActorRegistryResetView *actor)
{
    s32 delta, absolute;
    struct SceneActorControlMotionState *state;
    {
        struct SceneActorControlMotionState *raw = SceneActorControl_GetMotionState(actor);
        state = raw;
    }
    if (IsUnavailable(actor)) {
        return;
    }
    {
        volatile u32 *stateFlags;
        u32 firstValue;
        {
            volatile u32 *firstFlags;
            u32 firstMask = 2;
            firstFlags = &state->flags3D4;
            firstValue = *firstFlags & firstMask;
            stateFlags = firstFlags;
        }
        if (Test(firstValue, ~0)) {
            actor->body.position.y = state->baseOffset + actor->field2EA;
            return;
        }
        if ((s16)actor->field2EA <= 259) {
            {
                u32 mask = 1;
                if (Test(*stateFlags, mask)) {
                    actor->body.position.y = ((struct SceneActorControlGroundView *)actor)->ground;
                    return;
                }
            }
            actor->field2EA += TruncateSine(gUnk_086149C4[state->phase], 9);
            if (state->phase <= 63) {
                state->phase++;
            }
        }
    }
    {
        s32 currentGround = ((struct SceneActorControlGroundView *)actor)->ground;
        s32 raisedY = actor->body.position.y - 260;
        delta = currentGround - raisedY;
    }
    absolute = delta >= 0 ? delta : -delta;
    if (absolute > 16) {
        if (delta < 0) {
            actor->field2EA -= 2;
        } else {
            actor->field2EA += 2;
        }
    }
    if (((struct SceneActorControlGroundView *)actor)->ground != state->previousGround) {
        actor->field2EA -=
            ((struct SceneActorControlGroundView *)actor)->ground - state->previousGround;
    }
    do {
        {
            u32 offset = actor->field2EA;
            {
                u32 ground = ((struct SceneActorControlGroundView *)actor)->ground;
                actor->body.position.y = actor->field2EA + ground;
            }
        }
    } while (0);
    {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->drawable;
        drawable->position.y -= TruncateSine(gUnk_086149C4[state->phase], 5);
    }
    if (state->phase > 64) {
        state->phase = 0;
    } else {
        state->phase += 8;
    }
    state->previousGround = ((struct SceneActorControlGroundView *)actor)->ground;
}
