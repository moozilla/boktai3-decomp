#ifndef GUARD_SCENE_ACTOR_PLACEMENT_H
#define GUARD_SCENE_ACTOR_PLACEMENT_H
#include "scene/actor/scene_actor_reset.h"
#include "scene/actor/scene_actor_terrain.h"
#include "link/session/room_actor.h"
/* Tile drawable flags relative to its embedded block at drawable +0x20. */
struct SceneActorPlacementTileFlags {
    u8 unknown0[8];
    u32 flags;
};
/* Reset/placement phase; resetControl220 is observed separately from savedTarget22C. */
void SceneActorRegistry_ResetRouteAndColliders(struct ChaseActor *);
void SceneActorRegistry_ResetRoute(struct ChaseActor *);
void SceneActorRegistry_RestoreActorDefaults(struct ChaseActor *);
void SceneActorPlacement_PlayVariantEventSound(struct RoomActor *, u32);
static inline u32 SceneActorPlacement_ReadFlags16(struct RoomActor *actor, u32 mask)
{
    return actor->motionFlags & mask;
}
static inline u32 SceneActorPlacement_ReadFlags32(struct RoomActor *actor, u32 mask)
{
    return actor->statusFlags & mask;
}
static inline u8 SceneActorPlacement_IsUnavailable(struct RoomActor *actor)
{
    if (actor) {
        if (SceneActorPlacement_ReadFlags16(actor, 0x40) ||
            SceneActorPlacement_ReadFlags32(actor, 0x20000) || actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline u8 SceneActorPlacement_TestFlags(const u16 *flags, u32 mask)
{
    if (*flags & mask)
        return 1;
    return 0;
}
static inline void SceneActorPlacement_SetFlags(u32 bits, struct RoomActor *actor)
{
    actor->placementFlags = bits | actor->placementFlags;
}
static inline void SceneActorPlacement_ClearFlags(u16 *flags, u32 mask)
{
    *flags &= mask;
}
static inline u8 SceneActorPlacement_TestValue(u32 flags, u32 mask)
{
    if (mask & flags)
        return 1;
    return 0;
}
static inline u8 SceneActorPlacement_TestMask(u32 mask, u32 flags)
{
    if (mask & flags)
        return 1;
    return 0;
}
void SceneActorPlacement_PlayCategoryResponseSound(struct ChaseActor *);
#endif
