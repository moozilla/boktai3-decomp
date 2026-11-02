#include "scene_actor_state.h"
#include "scene_actor_route.h"
#include "script_utility.h"
void SceneActorMovement_ResolveRoutePosition(struct SceneActorRegistryActor *);
extern const u8 gUnk_08610814[];
static inline u8 *SceneActorRoute_BehaviorField(struct SceneActorRegistryActor *actor)
{
    return &actor->behavior2AA;
}

static inline void SceneActorRoute_StartWait(struct SceneActorRegistryActor *actor)
{
    u32 behavior = 2;
    u8 *field;
    u32 zero;
    field = &actor->state2A8;
    zero = 0;
    *field = zero;

    actor->phase2A9 = zero;

    *SceneActorRoute_BehaviorField(actor) = behavior;
    actor->counter2AC = zero;

    actor->field2B1 = 1;
}

void SceneActorRoute_AdvanceActorPoint(struct SceneActorRegistryActor *actor,
                                       struct SceneActorRouteCursor *cursor)
{
    struct SceneActorRouteCursor *embedded = (void *)((u8 *)actor + 0x448);
    u16 script = (u16)((struct SceneActorRegistryResetView *)actor)->field1EC;
    u32 mode, near;
    if (script) {
        s32 values[3];
        struct ScriptUtilityArguments args;
        values[0] = embedded->pointIndex;
        values[1] = actor->id;
        values[2] = actor->category;
        /* The shared argument convention leaves the unused halfword unspecified. */
        args.count = 3;
        args.values = values;
        Script_Call(script, &args);
    }
    Route_AdvancePoint(cursor);
    mode = cursor->point->attributes >> 4;
    near = (u8)SceneActorRoute_IsOutsidePointBounds(cursor, &actor->position, 32, 32);
    if (near) {
        SceneActorMovement_ResolveRoutePosition(actor);
        SceneActorRoute_StartWait(actor);
        actor->field2C2 = 64;
    } else if (gUnk_08610814[mode] == 1) {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 1;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
        {
            u32 *flags = &actor->flags2D0;
            u32 mask = ~0x20;
            u32 value;
            value = *flags;
            value &= mask;
            *flags = value;
        }
    } else {
        actor->state2A8 = 0;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
}
