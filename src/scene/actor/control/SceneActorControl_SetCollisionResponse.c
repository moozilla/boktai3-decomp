#include "scene/actor/scene_actor_control_helpers.h"
void SceneActorControl_SetCollisionResponse(struct ChaseActor *input)
{
    s16 k2 = 0;
    u8 *field_2;
    struct ChaseActor *actor = input;
    struct SceneActorChaseTracking *tracking = &actor->base.tracking;
    u32 statusMask = 8;
    u32 testMask;
    u16 *field;
    statusMask |= tracking->flags;
    tracking->flags = statusMask;
    testMask = 0x100;
    field = &actor->base.motionFlags;
    if (!(*field & testMask)) {
        struct SceneActorControlMotionState *state = SceneActorControl_GetMotionState(actor);
        u32 mask = 2;
        u32 *flags = &state->flags3D4;
        if (SceneActorControl_TestFlags(*flags, mask)) {
            u32 mask = 0x100;
            u16 *flags;
            u32 old;
            u32 zeroByte;
            u32 zeroWord;
            flags = &actor->base.motionFlags;
            old = *flags;
            zeroByte = k2;
            zeroWord = 0;
            mask |= old;
            *flags = mask;
            {
                u32 behavior = 3;
                u8 *field;
                field_2 = &actor->base.state2A8;
                *field_2 = zeroByte;
                field = &actor->base.phase2A9;
                *field = zeroByte;
                field = &actor->base.behavior2AA;
                *field = behavior;
                actor->base.counter2AC = zeroWord;
            }
            {
                u8 *field = &actor->base.field2B1;
                *field = 1;
            }
        }
    }
}
