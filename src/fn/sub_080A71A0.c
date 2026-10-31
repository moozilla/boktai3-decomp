#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"

struct TentacleTrailState {
    u8 unknown0[0x8EA];
    u8 segmentCount;
    u8 healthPerSegment;
};
s32 ActorTrail_RemoveLastSegment(struct SceneActorRegistryResetView *);

static inline s32 Tentacle_Shift(s32 value, u32 bits)
{
    s32 result;
    if (value >= 0)
        result = value >> bits;
    else
        result = -((-value) >> bits);
    return result;
}

static inline s32 Tentacle_ClampStep(s32 value, s16 limit)
{
    if (value > limit)
        return limit;
    if (value < -limit)
        return -limit;
    return value;
}

static inline void Tentacle_SetReactionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    actor->flags2D0 |= bits;
}

static inline u8 Tentacle_ApproachReactionEnd(struct SceneActorRegistryResetView *actor)
{
    s32 x, z;
    SceneActorChase_SetActorFlags(actor, 0x2000);
    Tentacle_SetReactionFlags(actor, 0x10000);
    x = Tentacle_Shift(actor->reactionStart.x - actor->reactionEnd.x, 2);
    z = Tentacle_Shift(actor->reactionStart.z - actor->reactionEnd.z, 2);
    x = Tentacle_ClampStep(x, 200);
    z = Tentacle_ClampStep(z, 200);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    actor->reactionEnd.x += x;
    actor->reactionEnd.z += z;
    if (x == 0 && z == 0)
        return 1;
    return 0;
}

static inline void Tentacle_PrepareRecoil(struct SceneActorRegistryResetView *actor, u16 bits)
{
    struct SceneActorChaseFlags16 *flags = (void *)&actor->flags2D4;
    SceneActorChase_SetFlagComponent(flags, bits);
    SceneActorChase_ClearFlagComponent(flags, 1);
}

void TentacleBody_ConvergeReactionMotion(struct SceneActorRegistryResetView *actor)
{
    struct TentacleTrailState *trail = actor->context;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        s32 segments;
        Tentacle_PrepareRecoil(actor, 0x20);
        segments = Div(actor->health, trail->healthPerSegment);
        for (; segments < trail->segmentCount; segments++)
            ActorTrail_RemoveLastSegment(actor);
    }
    if (Tentacle_ApproachReactionEnd(actor)) {
        actor->field2B0 = 1;
        SceneActorChase_ClearMotionFlagComponent(actor, 0x20);
        SceneActorChase_SetControl(actor, SceneActorControl_OnIdle, 16);
        SceneActorChase_SetState(actor, 0, 0, 5);
        actor->body.direction = actor->tracking.heading;
        actor->field2C2 = 64;
    }
}
