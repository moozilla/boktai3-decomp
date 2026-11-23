#include "scene_actor_control_motion.h"
#include "player.h"
static inline u32 ActorFlag(struct ChaseActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}
static inline u8 IsUnavailable(struct ChaseActor *actor)
{
    if (actor && (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) ||
                  actor->base.health <= 0))
        return 1;
    return 0;
}
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
struct LiftParameters {
    u8 unknown0[0xF];
    u8 flags : 8;
};
void SceneActorControl_ConsumeLiftFlag(struct ChaseActor *actor)
{
    struct SceneActorControlMotionState *state;
    if (IsUnavailable(actor))
        return;
    if (!gPlayers[0])
        return;
    state = SceneActorControl_GetMotionState(actor);
    if (LiftFlag(state)) {
        u32 *flags = &state->flags3D4;
        u32 clearMask;
        struct LiftParameters *parameters;
        clearMask = ~2;
        *flags &= clearMask;
        parameters = (void *)((u8 *)actor + 0xDC);
        {
            u8 bit = 8;
            parameters->flags = bit | parameters->flags;
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
