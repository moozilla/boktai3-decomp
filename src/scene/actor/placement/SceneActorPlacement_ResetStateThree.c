#include "scene/motion/scene_actor_chase_shared.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene/actor/scene_actor_placement.h"
static inline void SetFlags(struct RoomActor *s, u16 mask)
{
    s->motionFlags = mask | s->motionFlags;
}
s32 SceneActorPlacement_ResetStateThree(struct RoomActor *s)
{
    if (s->hitReactionFlags & 0x40000) {
        SetFlags(s, 0x20);
    }
    {
        SceneActorTerrain_InstallControl((struct ChaseActor *)s, s->onReaction, 0x12);
        SceneActorTerrain_EnterState((struct ChaseActor *)s, 3, 0, 0);
        SceneActorChase_AndWordFlags(&s->statusFlags, 0xFEFFFFFF);
        SceneActorPlacement_ClearFlags(&s->reactionFlags, 0xFFFFF636);
    }
    return 1;
}
