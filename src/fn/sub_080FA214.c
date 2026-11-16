#include "global.h"
// Remaining: counter offset is kept in sb across Mod rather than rematerialized at the final store.
#include "rotation_phase.h"

s32 Mod(s32, s32);
struct DampedMotionState {
    u8 unknown0[0xF14];
    s16 x;
    u8 unknownF16[2];
    s16 z;
};
void SceneActorControl_ApplyDampedMotion(struct ChaseActor *actor)
{
    struct DampedMotionState *state = actor->base.context;
    if (SceneActorControl_TakeFlag(&actor->base.field2B1))
        RotationPhase_Change(actor, actor->base.onEntry, 2);
    actor->base.body.motion.x += state->x;
    actor->base.body.motion.z += state->z;
    if (Mod(actor->base.counter2AC, 4) == 0) {
        state->x >>= 1;
        state->z >>= 1;
    }
    if (actor->base.stepDone) {
        u32 offset = 0x2A8;
        u8 *field = ((u8 *)actor) + offset;
        *field = 0;
        offset = 0x2A9;
        *(((u8 *)actor) + offset) = 0;
        offset = 0x2AA;
        *(((u8 *)actor) + offset) = 0;
        offset = 0x2AC;
        *((u32 *)(((u8 *)actor) - (-offset))) = 0;
        offset = 0x2B1;
        *(((u8 *)actor) + offset) = 1;
    } else {
        ++actor->base.counter2AC;
    }
}
