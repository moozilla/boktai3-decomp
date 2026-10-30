#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_route.h"
#include "scene_actor_terrain.h"
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
void sub_080C47B8(struct ContactPosition *);
void SceneActorRoute_SetReachableDestination(struct SceneActorRegistryResetView *actor,
                                             struct ContactPosition *destination)
{
    struct SceneActorRouteContext *context = actor->context;
    u8 *group = (u8 *)actor + 0x3D4;
    s32 cell = SceneActorTerrain_GetCellIndex(destination);
    u32 flags = *(u16 *)(SceneActorTerrain_GetCell(cell) + 2);
    u32 mask = 0x43;
    if (!(flags & mask)) {
        if (Nav_IsPositionInMesh(*(void **)(group + 0x1FC), &actor->body.position, destination)) {
            context->destination = *destination;
            sub_080C47B8(&context->destination);
            {
                u32 mask = 0x80000;
                context->flags628 |= mask;
            }
        }
    }
}
