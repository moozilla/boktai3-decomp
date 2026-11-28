#include "engine/math/math_shift.h"
#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_control_motion.h"
extern const s16 gUnk_086149C4[];
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActorControl_TickGroundBob(struct ChaseActor *actor)
{
    s32 delta;
    struct SceneActorControlMotionState *state;
    state = SceneActorControl_GetMotionState(actor);
    if (SceneActor_ActionsBlocked(actor))
        return;
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
                    actor->base.body.position.y = actor->base.groundHeight;
                    return;
                }
            }
            actor->base.heightOffset += Math_ShiftTowardZero(gUnk_086149C4[state->phase], 9);
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
    if (abs(delta) > 16) {
        if (delta < 0) {
            actor->base.heightOffset -= 2;
        } else {
            actor->base.heightOffset += 2;
        }
    }
    if (actor->base.groundHeight != state->previousGround)
        actor->base.heightOffset -= actor->base.groundHeight - state->previousGround;
    actor->base.body.position.y = actor->base.groundHeight + actor->base.heightOffset;
    {
        struct SceneActorRegistrySequenceDrawable *drawable = actor->base.drawable;
        drawable->position.y -= Math_ShiftTowardZero(gUnk_086149C4[state->phase], 5);
    }
    if (state->phase > 64) {
        state->phase = 0;
    } else {
        state->phase += 8;
    }
    state->previousGround = actor->base.groundHeight;
}
