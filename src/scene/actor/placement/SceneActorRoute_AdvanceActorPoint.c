#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_state.h"
#include "scene/motion/scene_actor_route.h"
#include "script/script_utility.h"
void SceneActorMovement_ResolveRoutePosition(struct ChaseActor *);
extern const u8 gUnk_08610814[];
void SceneActorRoute_AdvanceActorPoint(struct ChaseActor *actor,
                                       struct SceneActorRouteCursor *cursor)
{
    struct SceneActorRouteCursor *embedded = (void *)&actor->group.homeRoute;
    u16 script = (u16)actor->base.scriptId;
    u32 mode, near;
    if (script) {
        s32 values[3];
        struct ScriptUtilityArguments args;
        values[0] = embedded->pointIndex;
        values[1] = actor->base.body.id;
        values[2] = actor->base.category;
        /* The shared argument convention leaves the unused halfword unspecified. */
        args.count = 3;
        args.values = values;
        Script_Call(script, &args);
    }
    Route_AdvancePoint(cursor);
    mode = cursor->point->attributes >> 4;
    near = (u8)SceneActorRoute_IsOutsidePointBounds(cursor, &actor->base.body.position, 32, 32);
    if (near) {
        SceneActorMovement_ResolveRoutePosition(actor);
        SceneActorTerrain_EnterState(actor, 0, 0, 2);
        actor->base.field2C2 = 64;
    } else if (gUnk_08610814[mode] == 1) {
        SceneActorTerrain_EnterState(actor, 0, 0, 1);
        {
            u32 *flags = &actor->base.statusFlags;
            u32 mask = ~0x20;
            u32 value;
            value = *flags;
            value &= mask;
            *flags = value;
        }
    } else {
        SceneActorTerrain_EnterState(actor, 0, 0, 0);
    }
}
