#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_control_motion.h"
#include "player/player.h"
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u8 LiftFlag(struct SceneActorControlMotionState *state)
{
    u32 mask = 2;
    return Test(state->flags3D4, mask);
}
struct LiftHalfBits {
    u16 bits : 16;
};
void SceneActorControl_ConsumeLiftFlag(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state;
    if (SceneActor_ActionsBlocked(actor))
        return;
    if (!gPlayers[0])
        return;
    state = SceneActorControl_GetMotionState(actor);
    if (LiftFlag(state)) {
        u32 *flags = &state->flags3D4;
        u32 clearMask;
        struct SceneActorChaseTracking *tracking;
        clearMask = ~2;
        *flags &= clearMask;
        tracking = &actor->base.tracking;
        {
            u8 bit = 8;
            tracking->flags = bit | tracking->flags;
        }
        {
            u32 bit = 0x100;
            struct LiftHalfBits *f = (void *)&actor->base.motionFlags;
            f->bits = bit | f->bits;
        }
        {
            u8 behavior = 3;
            actor->base.state2A8 = 0;
            actor->base.phase2A9 = 0;
            actor->base.behavior2AA = behavior;
            actor->base.counter2AC = 0;
            actor->base.field2B1 = 1;
        }
    }
}
