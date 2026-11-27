#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_control_motion.h"
#include "bosses/vanargand/vanargand_parts.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActorControl_TickGroundBob(struct ChaseActor *actor)
{
    s32 delta, absolute;
    struct SceneActorControlMotionState *state;
    {
        struct SceneActorControlMotionState *raw = SceneActorControl_GetMotionState(actor);
        state = raw;
    }
    if (SceneActor_ActionsBlocked(actor)) {
        return;
    }
    {
        u32 *stateFlags;
        u32 firstValue;
        {
            u32 *firstFlags;
            u32 firstMask = 2;
            firstFlags = &state->flags3D4;
            firstValue = *firstFlags & firstMask;
            stateFlags = firstFlags;
        }
        if (Test(firstValue, ~0)) {
            actor->base.body.position.y = state->baseOffset + actor->base.heightOffset;
            return;
        }
        if ((s16)actor->base.heightOffset <= 259) {
            {
                u32 mask = 1;
                if (Test(*stateFlags, mask)) {
                    actor->base.body.position.y =
                        actor->base.groundHeight;
                    return;
                }
            }
            actor->base.heightOffset += Vanargand_TruncateShift(gUnk_086149C4[state->phase], 9);
            if (state->phase <= 63) {
                state->phase++;
            }
        }
    }
    {
        s32 currentGround = actor->base.groundHeight;
        s32 raisedY = actor->base.body.position.y - 260;
        delta = currentGround - raisedY;
    }
    absolute = delta >= 0 ? delta : -delta;
    if (absolute > 16) {
        if (delta < 0) {
            actor->base.heightOffset -= 2;
        } else {
            actor->base.heightOffset += 2;
        }
    }
    if (actor->base.groundHeight != state->previousGround) {
        actor->base.heightOffset -=
            actor->base.groundHeight - state->previousGround;
    }
    do {
        {
            u32 offset = actor->base.heightOffset;
            {
                u32 ground = actor->base.groundHeight;
                actor->base.body.position.y = actor->base.heightOffset + ground;
            }
        }
    } while (0);
    {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->base.drawable;
        drawable->position.y -= Vanargand_TruncateShift(gUnk_086149C4[state->phase], 5);
    }
    if (state->phase > 64) {
        state->phase = 0;
    } else {
        state->phase += 8;
    }
    state->previousGround = actor->base.groundHeight;
}
