#include "scene/motion/scene_actor_availability.h"
#include "scene/actor/scene_actor_placement.h"
#include "scene/actor/scene_actor_init_actor.h"
#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
void SceneActorAttachment_BeginReturnResponse(struct ChaseActor *);
s32 SceneActorBehavior_DispatchPendingFlags(struct ChaseActor *actor)
{
    u32 unavailable = SceneActor_ActionsBlocked((void *)actor);
    if (!unavailable && actor->base.reactionFlags) {
        struct ChaseActor *self = actor;
        struct SceneActorChaseGroup *group = &actor->group;
        u16 *flags;
        if (SceneActor_TestPendingFlag((void *)actor, 4, &flags)) {
            SceneActorChase_ClearBits(flags, 0x987);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 2)) {
            SceneActorTerrain_EnterState(actor, 3, 0, 0);
            SceneActorTerrain_InstallControl(actor, group->contactControl, 7);
            {
                s32 value = actor->base.attachmentRestart;
                actor->base.attachmentTimer = value;
            }
            SceneActorChase_ClearBits(flags, 0x983);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 0x800)) {
            SceneActorAttachment_BeginReturnResponse((void *)self);
            SceneActorChase_ClearBits(flags, 0x981);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 1)) {
            SceneActorChase_ClearBits(flags, 0x181);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 0x80)) {
            void *callback = actor->base.reactionGate;
            if (callback)
                Callback_InvokeActor(actor, callback);
            SceneActorChase_ClearBits(flags, 0x180);
            return 1;
        }
        if (SceneActorPlacement_TestFlags(flags, 0x100)) {
            void *callback = actor->group.routeRestoreCallback;
            if (callback)
                Callback_InvokeActor(actor, callback);
            SceneActorChase_ClearBits(flags, 0x180);
            return 1;
        }
    }
    return 0;
}
