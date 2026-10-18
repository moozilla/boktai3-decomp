#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_route.h"
#include "scene_actor_terrain.h"
s32 sub_0821F204(void *, struct ContactPosition *, struct ContactPosition *);
void sub_080C47B8(struct ContactPosition *);
void SceneActorRoute_CommitReachableDestination(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorRouteContext *context = actor->context;
    u8 *group = (u8 *)actor + 0x3D4;
    struct SceneActorRouteDestination *target = (void *)actor->fieldDC;
    s32 cell = SceneActorTerrain_GetCellIndex(&target->position);
    u32 flags = *(u16 *)(SceneActorTerrain_GetCell(cell) + 2);
    u32 mask = 0x43;
    if (!(flags & mask)) {
        if (sub_0821F204(*(void **)(group + 0x1FC), &actor->position,
                         &((struct SceneActorRouteDestination *)actor->fieldDC)->position)) {
            context->destination = ((struct SceneActorRouteDestination *)actor->fieldDC)->position;
            sub_080C47B8(&context->destination);
        }
    }
}
