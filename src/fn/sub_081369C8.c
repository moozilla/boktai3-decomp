#include "scene_actor_terrain_reaction.h"
#include "global.h"
#include "scene_actor_placement.h"
static inline void SetFlags(struct RoomActor *s, u16 mask)
{
    s->motionFlags = mask | s->motionFlags;
}
static inline void ClearFlags32(u32 *p, u32 mask)
{
    *p &= mask;
}
s32 SceneActorPlacement_ResetStateThree(struct RoomActor *s)
{
    if (s->hitReactionFlags & 0x40000) {
        SetFlags(s, 0x20);
    }
    {
        SceneActorTerrain_InstallControl((struct ChaseActor *)s, s->onReaction, 0x12);
        SceneActorTerrain_EnterState((struct ChaseActor *)s, 3, 0, 0);
        ClearFlags32(&s->statusFlags, 0xFEFFFFFF);
        SceneActorPlacement_ClearFlags(&s->reactionFlags, 0xFFFFF636);
    }
    return 1;
}
